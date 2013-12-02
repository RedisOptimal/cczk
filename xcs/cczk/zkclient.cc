#include <zkclient.h>

#include <boost/shared_ptr.hpp>

#include <watcher.h>
#include <logging.h>

namespace xcs {
namespace cczk {
DEFINE_int32(xcs_zk_node_max_length,
             1024*1024,
             "Default length limitation of zookeeper node's data is 1M.");
DEFINE_int32(refresh_timeval,
             300,
             "Default refresh timeval.");

ZkClient::ZkClient():
  zhandle_(NULL),
  background_watcher_(true),
  background_watcher_thread_(boost::bind(&ZkClient::WatcherLoop, this)) {
  srand(getpid());
  Clear();
}
void ZkClient::UpdateAuth() {
  if (zhandle_ != NULL) {
    const string scheme = "digest";
    for (std::map<string, string>::iterator it = config_.auth.begin();
          it != config_.auth.end();
          ++it) {
      string certification = it->first+":"+it->second;
      int rc = zoo_add_auth(zhandle_,
                            scheme.c_str(),
                            certification.c_str(),
                            certification.size(),
                            NULL,
                            NULL);
      if (rc != ZOK) {
        XCS_WARN << "[UPDATE AUTH]RETCODE=" << zerror(rc)
        << "@" << certification;
      }
    }
  }
}
zhandle_t* ZkClient::CreateConnection() {
  if (zhandle_ != NULL) {
    zookeeper_close(zhandle_);
    zhandle_ = NULL;
  }
#ifndef DEBUG
  sleep(rand() % 10);
#endif
  FILE *zk_log = fopen("zookeeper.out", "w");
  if (zk_log != NULL) {
    zoo_set_log_stream(zk_log);
  } else {
    XCS_ERROR << "Can't create zk log file.";
  }
  zhandle_ = zookeeper_init((config_.get_host()+"/"+config_.get_root()).c_str(),
                            ZkClient::init_watcher,
                            config_.get_session_timeout(),
                            NULL,
                            static_cast<void*>(this),
                            0);
  if (zhandle_ == NULL) {
    return zhandle_;
  }
  int sleep_interval = 1;
  while (zoo_state(zhandle_) != ZOO_CONNECTED_STATE) {
    sleep(sleep_interval);
    if (sleep_interval > config_.get_session_timeout()/1000) {
      zookeeper_close(zhandle_);
      zhandle_ = NULL;
      break;
    }
    sleep_interval <<= 1;
  }
  return zhandle_;
}
void ZkClient::WatcherLoop() {
  while (background_watcher_) {
    sleep(FLAGS_refresh_timeval);
    {
      if (!IsAvailable()) continue;
      boost::recursive_mutex::scoped_lock lock(background_mutex_);
      TriggerAllWatcher(listeners_);
    }
  }
}
void ZkClient::Clear() {
  boost::recursive_mutex::scoped_lock lock(background_mutex_);
  ListenerMap::iterator it;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    it->first->close();
  }
}
ZkClient::~ZkClient() {
  background_watcher_ = false;
  background_watcher_thread_.join();
  zookeeper_close(zhandle_);
  zhandle_ = NULL;
}
void ZkClient::TriggerAllWatcher(const ZkClient::ListenerMap& listeners_) {
  ListenerMap::iterator it;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    ListenerMapKey _key = it->first;
    ListenerMapValue _value = it->second;
    if (_key->is_live()) {
      ListenerMapValue::iterator jt;
      for (jt = _value.begin(); jt != _value.end(); ++jt) {
        if (_key->watch_child()) {
          event_watcher(this->zhandle_,
                        WatchEvent::ZnodeChildrenChanged,
                        0,
                        jt->first.c_str(),
                        reinterpret_cast<void*>(&_key));
        }
        if (_key->watch_data()) {
          event_watcher(this->zhandle_,
                        WatchEvent::ZnodeDataChanged,
                        0,
                        jt->first.c_str(),
                        reinterpret_cast<void*>(&_key));
        }
      }
    }
  }
}
bool ZkClient::IsAvailable()  {
  boost::mutex::scoped_lock lock(this->singleton_mutex_);
  if (zhandle_ != NULL &&
    static_cast<SessionState::type>(zoo_state(zhandle_)) == SessionState::Connected) {
    return true;
  }
  return false;
}
void ZkClient::Close() {
  boost::mutex::scoped_lock lock(this->singleton_mutex_);
  boost::recursive_mutex::scoped_lock lock2(this->background_mutex_);
  if (zhandle_ != NULL) {
    zookeeper_close(zhandle_);
    zhandle_ = NULL;
  }
}
void ZkClient::init_watcher(zhandle_t* zh, int type,
      int state, const char* path, void* watcherCtx) {
  ZkClient *instance = static_cast<ZkClient*>(watcherCtx);
  if (type == WatchEvent::SessionStateChanged) {
    if (state == SessionState::Expired) {
      instance->Close();
      while (NULL == instance->CreateConnection()) {
        XCS_ERROR << "[INIT_WATCHER] Can't create connection.";
        sleep(5);
      }
    } else if (state == SessionState::Connecting) {
      XCS_WARN << "[ZK CONNECTION NOT STABILITY]";
    } else if (state == SessionState::Connected) {
      XCS_INFO << "Connection with zk";
    }
    if (state != SessionState::Connecting) {
      instance->TriggerAllWatcher(instance->listeners_);
    }
  }
}
void ZkClient::event_watcher(zhandle_t* zh, int type,
      int state, const char* path, void* watcherCtx) {
  boost::shared_ptr<watcher> *point =
    static_cast<boost::shared_ptr<watcher>* >(watcherCtx);
  ZkClient *instance = ZkClient::Open();
  string temp_path(path);
  WatchEvent::type temp_state;
  if (type != ZOO_SESSION_EVENT) {
    temp_state = static_cast<WatchEvent::type>(type);
    if (temp_state == WatchEvent::ZnodeRemoved ||
        temp_state == WatchEvent::ZnodeCreated) {
      temp_state = WatchEvent::ZnodeDataChanged;
    }
  }
  boost::recursive_mutex::scoped_lock lock(instance->background_mutex_);
  //  already droped by user
  ListenerMap::iterator it;
  //  check wather in listener
  if ((it = instance->listeners_.find(*point)) == instance->listeners_.end()) {
    XCS_ERROR << "[EVENT WATCHER]Wather not Exist anymore";
    return;
  }
  ListenerMapValue::iterator jt;
  if (!it->first->is_live()) {  //  watcher die
    //  watcher die and remove path
    if ((jt = it->second.find(temp_path)) != it->second.end()) {
      if (temp_state == WatchEvent::ZnodeDataChanged) {
        jt->second.first = false;
      } else if (temp_state == WatchEvent::ZnodeChildrenChanged) {
        jt->second.second = false;
      }
      if (!jt->second.first && !jt->second.second) {
        it->second.erase(jt);
        if (it->second.empty()) {
          instance->listeners_.erase(it);
        }
      }
    }
    return;
  } else {  //  watcher live
    //  watcher path droped
    if ((jt = it->second.find(temp_path)) == it->second.end()) {
      return;
    }
  }
  //  register back to zk
  ReturnCode::type ret;
  ret = instance->AddListener(*point, temp_path);
  if (ret != ReturnCode::Ok) {
    XCS_ERROR << "[EVENT WATCHER]RETCODE=" << ReturnCode::toString(ret)
    << "@PATH=" << temp_path << " WATCHER=" << point;
  }
  //  trigger the callback function
  if (type != ZOO_SESSION_EVENT) {
    point->get()->get_listener()(temp_path, temp_state);
  }
}
ZkClient* ZkClient::Open(const ZookeeperConfig *config)  {
  static ZkClient instance;
  boost::mutex::scoped_lock lock(instance.singleton_mutex_);
  if (config == NULL) {
    return &instance;
  }
  if (*config != instance.config_) {
    instance.config_ = *config;
    if (NULL == instance.CreateConnection()) {
      return NULL;
    }
    instance.UpdateAuth();
    return &instance;
  }
  if (*config == instance.config_) {
    instance.config_ = *config;
    if (NULL == instance.zhandle_) {
      if (NULL == instance.CreateConnection()) {
        return NULL;
      }
    }
    instance.UpdateAuth();
    return &instance;
  }
  return NULL;
}
ReturnCode::type ZkClient::GetChildrenOfPath(const string path,
                                             std::vector< string >& children) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  Stat stat;
  ReturnCode::type return_code;
  if ((return_code = Exist(path)) != ReturnCode::Ok) {
    XCS_ERROR << "[GET CHILDREN OF PATH]RETCODE=" <<
    ReturnCode::toString(return_code) << "@PATH=" << path;
    return return_code;
  }
  String_vector string_vector;
  int rc = zoo_get_children(this->zhandle_, path.c_str(), 0, &string_vector);
  if (rc != ZOK) {
    XCS_ERROR << "[GET CHILDREN OF PATH]RETCODE=" <<
    ReturnCode::toString(return_code) << "@PATH=" << path;
    return_code = static_cast<ReturnCode::type>(rc);
  } else {
    children.clear();
    for (int i = 0; i < string_vector.count; ++i) {
      children.push_back(string_vector.data[i]);
    }
  }
  deallocate_String_vector(&string_vector);
  return return_code;
}
ReturnCode::type ZkClient::SetDataOfNode(const string path, string& value) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  if (value.length() > (uint32_t)FLAGS_xcs_zk_node_max_length) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = Exist(path)) != ReturnCode::Ok) {
    return return_code;
  }
  int rc;
  if (value.length() == 0) {
    rc = zoo_set(zhandle_, path.c_str(), NULL, -1, -1);
  } else {
    rc = zoo_set(zhandle_, path.c_str(), value.c_str(), value.length(), -1);
  }
  if (rc != ZOK) {
    return_code = static_cast<ReturnCode::type>(rc);
  }
  return return_code;
}
ReturnCode::type ZkClient::GetDataOfNode(const string path, string& value) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = Exist(path)) != ReturnCode::Ok) {
    XCS_ERROR << "[GET DATA OF NODE]RETCODE=" <<
    ReturnCode::toString(return_code) << "@PATH=" << path;
    return return_code;
  }
  int length = FLAGS_xcs_zk_node_max_length;
  boost::scoped_array<char> buffer(new char[FLAGS_xcs_zk_node_max_length]);
  Stat stat;
  int rc = zoo_get(zhandle_,
                   path.c_str(),
                   0,
                   buffer.get(),
                   &length,
                   &stat);
  if (rc != ZOK || length == -1) {
    value.clear();
    return_code = static_cast<ReturnCode::type>(rc);
  } else {
    value.assign(buffer.get(), stat.dataLength);
  }
  return return_code;
}
ReturnCode::type ZkClient::Exist(const string path) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  Stat stat;
  int rc = zoo_exists(this->zhandle_, path.c_str(), 0, &stat);
  if (rc != ZOK) {
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}
ReturnCode::type ZkClient::CreateNode(const string path,
                                      const string& data,
                                      CreateMode::type mode) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  if (data.length() > (uint32_t)FLAGS_xcs_zk_node_max_length) {
    return ReturnCode::Error;
  }
  int rc;
  if (data.length() == 0) {
    rc = zoo_create(zhandle_,
                    path.c_str(),
                    NULL,
                    -1,
                    &ZOO_OPEN_ACL_UNSAFE,
                    mode,
                    NULL,
                    0);
  } else {
    rc = zoo_create(zhandle_,
                    path.c_str(),
                    data.c_str(),
                    data.length(),
                    &ZOO_OPEN_ACL_UNSAFE,
                    mode,
                    NULL,
                    0);
  }
  if (rc != ZOK) {
    XCS_ERROR << "[CREATE NODE]RETCODE=" << zerror(rc)
    << "@PATH=" << path << " DATA=" << data << " MODE="
    << CreateMode::toString(mode);
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}
ReturnCode::type ZkClient::DeleteNode(const string path) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  int rc = zoo_delete(zhandle_, path.c_str(), -1);
  if (rc != ZOK) {
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}
ReturnCode::type ZkClient::AddListener(boost::shared_ptr< watcher > listener,
                                       string path) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = Exist(path)) != ReturnCode::Ok) {
    return return_code;
  }
  boost::recursive_mutex::scoped_lock lock(background_mutex_);
  ListenerMap::iterator it;
  //  add listener to local data-struct
  if ((it = listeners_.find(listener)) != listeners_.end()) {  //  already in map
    if (!it->first->is_live()) {  //  removed in past
      return ReturnCode::Error;
    } else {
      PreprotyOfPath _preproty = std::make_pair(listener->watch_data(),
                                                listener->watch_child());
      it->second.insert(std::make_pair(path, _preproty));
    }
  } else {  //  not in map
    ListenerMapKey _key = listener;
    ListenerMapValue _value;
    PreprotyOfPath _preproty = std::make_pair(_key->watch_data(), _key->watch_child());
    _value.insert(std::make_pair(path, _preproty));
    listeners_.insert(std::make_pair(_key, _value));
  }
  it = listeners_.find(listener);
  if (it->first->watch_data()) {
    Stat stat;
    int rc1 = zoo_wexists(zhandle_,
                        path.c_str(),
                        ZkClient::event_watcher,
                        reinterpret_cast<void*>(&(it->first)),
                        &stat);
    if (rc1 != ZOK) {
      XCS_ERROR << "[ADD LISTENER]RETCODE=" << zerror(rc1) <<
      "@PATH=" << path << " Watcher=" << listener;
      return_code = static_cast<ReturnCode::type>(rc1);
    }
  }
  if (it->first->watch_child()) {
    String_vector string_vector;
    int rc2 = zoo_wget_children(zhandle_,
                                path.c_str(),
                                ZkClient::event_watcher,
                                reinterpret_cast<void*>(&(it->first)),
                                &string_vector);
    deallocate_String_vector(&string_vector);
    if (rc2 != ZOK) {
      XCS_ERROR << "[ADD LISTENER]RETCODE=" << zerror(rc2) <<
      "@PATH=" << path << " Watcher=" << listener;
      return_code = static_cast<ReturnCode::type>(rc2);
    }
  }
  return return_code;
}
ReturnCode::type ZkClient::DropListenerWithPath(boost::shared_ptr< watcher > listener, string path) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  boost::recursive_mutex::scoped_lock lock(background_mutex_);
  ListenerMap::iterator it;
  if ((it = listeners_.find(listener)) != listeners_.end()) {
    ListenerMapValue::iterator jt;
    if ((jt = it->second.find(path)) != it->second.end()) {
      it->second.erase(jt);
    }
    return ReturnCode::Ok;
  } else {
    return ReturnCode::Error;
  }
  //  never go here
  return ReturnCode::Error;
}
ReturnCode::type ZkClient::DropListener(boost::shared_ptr< watcher > listener) {
  if (!this->IsAvailable()) {
    return ReturnCode::Error;
  }
  boost::recursive_mutex::scoped_lock lock(background_mutex_);
  ListenerMap::iterator it;
  if ((it = listeners_.find(listener)) != listeners_.end()) {  //  already in map
    if (!it->first->is_live()) {  //  removed in past
      return ReturnCode::Ok;
    } else {
      it->first->close();
      return ReturnCode::Ok;
    }
  } else {  //  not in map
    return ReturnCode::Error;
  }
  //  never go here
  return ReturnCode::Error;
}

}  //  namespace cczk
}  //  namespace xcs
