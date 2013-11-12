#include <zkclient.h>

#include <stdio.h>
#include <errno.h>

#include <boost/shared_ptr.hpp>

#include <watcher.h>

namespace cczk {
DEFINE_int32(xcs_zk_node_max_length, 1024*1024, "Default length limitation of zookeeper node's data is 1M.");
DEFINE_int32(refresh_timeval, 1, "Default refresh timeval.");

zkclient::zkclient(): _zhandle(NULL),
            _background_watcher(true),
            _background_watcher_thread(boost::bind(&zkclient::watcher_loop, this)) {
  srand(getpid());
  clear();
}
    
void zkclient::update_auth() {
  if (_zhandle != NULL) {
    const string scheme = "digest";
    for (std::map<string, string>::iterator it = _config.auth.begin();
          it != _config.auth.end();
          ++it) {
      string certification = it->first+":"+it->second;
      int rc = zoo_add_auth(_zhandle,
                            scheme.c_str(),
                            certification.c_str(),
                            certification.size(),
                            NULL,
                            NULL
                            );
      if (rc != ZOK) {
        //TO-DO log 
      }
    }
  } 
} 

zhandle_t* zkclient::create_connection() {
  if (_zhandle != NULL) {
    zookeeper_close(_zhandle);
    _zhandle = NULL; 
  }
#ifndef DEBUG
  sleep(rand() % 10);
#endif
  FILE *zk_log = fopen("zookeeper.out","w");
  zoo_set_log_stream(zk_log);
  _zhandle = zookeeper_init((_config.get_host()+"/"+_config.get_root()).c_str(),
                            zkclient::init_watcher,
                            _config.get_session_timeout(),
                            NULL,
                            static_cast<void*>(this),
                            0);
  if (_zhandle == NULL) {
    return _zhandle;
  }
  int sleep_interval = 1;
  while (zoo_state(_zhandle) != ZOO_CONNECTED_STATE) {
    sleep(sleep_interval);
    if (sleep_interval > _config.get_session_timeout()/1000) {
      zookeeper_close(_zhandle);
     _zhandle = NULL; 
      break;
    }
    sleep_interval <<= 1;
  }
  return _zhandle;
}

void zkclient::watcher_loop() {
  while (_background_watcher) {
    sleep(FLAGS_refresh_timeval);
    {
      if (!is_avaiable()) continue;
      boost::mutex::scoped_lock lock(background_mutex);
      listener_map::iterator it;
      for (it = _listeners.begin(); it != _listeners.end(); ++it) {
        listener_map_key _key = it->first;
        listener_map_value _value = it->second;
        if (_key->is_live()) {
          listener_map_value::iterator jt;
          for (jt = _value.begin();jt != _value.end(); ++jt) {
            //because of background_mutex, do not use add_listener()
            Stat stat;
            int rc = zoo_wexists(_zhandle,
                                jt->c_str(),
                                zkclient::event_watcher,
                                static_cast<void*>(&_key),
                                &stat);
            if (rc != ZOK) {
              //TO-DO log
            }
          }
        }
      }
      std::map<string ,pair<string, CreateMode::type> >::iterator jt;
      for (jt = _ephemeral_node.begin();jt != _ephemeral_node.end(); ++jt) {
        create_node(jt->first, jt->second.first, jt->second.second);
      }
    }
  }
}
 
void zkclient::clear() {
  boost::mutex::scoped_lock lock(background_mutex);
  listener_map::iterator it;
  for (it = _listeners.begin();it != _listeners.end(); ++it) {
    it->first->close();
  }
  _ephemeral_node.clear();
}

 
zkclient::~zkclient() {
  _background_watcher = false;
  _background_watcher_thread.join();
  zookeeper_close(_zhandle);
  _zhandle = NULL; 
}

 
bool zkclient::is_avaiable()  {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
  if (_zhandle != NULL && 
    static_cast<SessionState::type>(zoo_state(_zhandle)) == SessionState::Connected) {
    return true;
  }
  return false;
}
 
void zkclient::close() {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
  if (_zhandle != NULL) {
    zookeeper_close(_zhandle);
    _zhandle = NULL; 
  }
}

void zkclient::init_watcher(zhandle_t* zh, int type, 
      int state, const char* path, void* watcherCtx) {
  zkclient *instance = static_cast<zkclient*>(watcherCtx);
  if (type == WatchEvent::SessionStateChanged) {
    if (state == SessionState::Expired) {
      instance->close();
      while (NULL == instance->create_connection()) {
        //TO-DO log   
      }
    } else if (state == SessionState::Connecting) {
        //TO-DO log   
    } else if (state == SessionState::Connected) {
        //TO-DO log
    }
  } 
}

void zkclient::event_watcher(zhandle_t* zh, int type, 
      int state, const char* path, void* watcherCtx) {
  boost::shared_ptr<watcher> *point = static_cast<boost::shared_ptr<watcher>* >(watcherCtx);
  zkclient *instance = zkclient::open(NULL);
  string temp_path(path);
  //already droped by user
  listener_map::iterator it;
  //check wather in listener
  if ((it = instance->_listeners.find(*point)) == instance->_listeners.end()) {  
    //TO-DO log bug
    return;
  }
  
  listener_map_value::iterator jt;
  if (!it->first->is_live()) {  //watcher die
    if ((jt = it->second.find(temp_path)) != it->second.end()) {  //watcher die and remove path
      it->second.erase(jt);
      if (it->second.empty()) {
        instance->_listeners.erase(it);
      }
    }
    return;
  } else {  //watcher live
    if ((jt = it->second.find(temp_path)) == it->second.end()) {  //watcher path droped
      return;
    }
  }
  
  //register back to zk
  instance->add_listener(*point, temp_path);
  
  //trigger the callback function
  WatchEvent::type temp_state = static_cast<WatchEvent::type>(state);
  point->get()->get_listener()(temp_path, temp_state);
}


zkclient* zkclient::open(const zookeeper_config *config)  {
  static zkclient instance;
  boost::mutex::scoped_lock lock(instance.singleton_mutex);
  if (config == NULL) {
    return &instance;
  }
  if (*config != instance._config) {
    instance._config = *config;
    if (NULL == instance.create_connection()) {
      return NULL;
    }
    instance.update_auth();
    return &instance;
  }
  if (*config == instance._config) {
    instance._config = *config;
    if (NULL == instance._zhandle) {
      if (NULL == instance.create_connection()) {
        return NULL;
      }
    }
    instance.update_auth();
    return &instance;
  }
  return NULL;
}
 
ReturnCode::type zkclient::get_children_of_path(const string path, std::vector< string >& children) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  Stat stat;
  ReturnCode::type return_code;
  if ((return_code = exist(path)) != ReturnCode::Ok) {
    //TO-DO log
    return return_code;
  }
  String_vector string_vector;
  int rc = zoo_get_children(this->_zhandle, path.c_str(), 0, &string_vector);
  if (rc != ZOK) {
    //TO-DO log
    return_code = static_cast<ReturnCode::type>(rc);
  } else {
    children.clear();
    for (int i = 0;i < string_vector.count; ++i) {
      children.push_back(string_vector.data[i]);
    }
  }
  deallocate_String_vector(&string_vector);
  return return_code;
}

ReturnCode::type zkclient::set_data_of_node(const string path, string& value) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  if (value.length() > (uint32_t)FLAGS_xcs_zk_node_max_length) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = exist(path)) != ReturnCode::Ok) {
    return return_code;
  }
  int rc;
  if (value.length() == 0) {
    rc = zoo_set(_zhandle, path.c_str(), NULL, -1, -1);
  } else {
    rc = zoo_set(_zhandle, path.c_str(), value.c_str(), value.length(), -1);
  }
  if (rc != ZOK) {
    return_code = static_cast<ReturnCode::type>(rc);
  }
  return return_code;
}


ReturnCode::type zkclient::get_data_of_node(const string path, string& value) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = exist(path)) != ReturnCode::Ok) {
    //TO-DO log
    return return_code;
  }
  int length = FLAGS_xcs_zk_node_max_length;
  boost::scoped_array<char> buffer(new char[FLAGS_xcs_zk_node_max_length]);
  Stat stat;
  int rc = zoo_get(_zhandle,
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
 
ReturnCode::type zkclient::exist(const string path) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  Stat stat;
  int rc = zoo_exists(this->_zhandle, path.c_str(), 0, &stat); 
  if (rc != ZOK) {
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}
 
ReturnCode::type zkclient::create_node(const string path, string& data, CreateMode::type mode) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  if (data.length() > (uint32_t)FLAGS_xcs_zk_node_max_length) {
    return ReturnCode::Error;
  }
  
  int rc;
  if (data.length() == 0) {
    rc = zoo_create(_zhandle,
                    path.c_str(),
                    NULL,
                    -1,
                    &ZOO_OPEN_ACL_UNSAFE,
                    mode,
                    NULL,
                    0);
  } else {
    rc = zoo_create(_zhandle,
                    path.c_str(),
                    data.c_str(),
                    data.length(),
                    &ZOO_OPEN_ACL_UNSAFE,
                    mode,
                    NULL,
                    0);
  }
  if (rc != ZOK) {
    //TO-DO log
    return static_cast<ReturnCode::type>(rc);
  }
  if (mode == CreateMode::Ephemeral) { 
    _ephemeral_node.insert(std::make_pair(path, std::make_pair(data, mode)));
  }
  return ReturnCode::Ok;
}
 
ReturnCode::type zkclient::delete_node(const string path) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  _ephemeral_node.erase(path);
  int rc = zoo_delete(_zhandle, path.c_str(), -1);
  if (rc != ZOK) {
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}

ReturnCode::type zkclient::add_listener(boost::shared_ptr< watcher > &listener, string path) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  ReturnCode::type return_code;
  if ((return_code = exist(path)) != ReturnCode::Ok) {
    return return_code;
  }
  boost::mutex::scoped_lock lock(background_mutex);
  listener_map::iterator it;
  //add listener to local data-struct 
  if ((it = _listeners.find(listener)) != _listeners.end()) {  //already in map
    if (!it->first->is_live()) {  //removed in past
      return ReturnCode::Error;
    } else {
      it->second.insert(path);
    }
  } else {  //not in map
    listener_map_key _key = listener;
    listener_map_value _value;
    _value.insert(path);
    _listeners.insert(std::make_pair(_key, _value));
  }
  Stat stat;
  int rc = zoo_wexists(_zhandle,
                       path.c_str(),
                       zkclient::event_watcher,
                       static_cast<void*>(&listener),
                       &stat);
  if (rc != ZOK) {
    //TO-DO log
    return_code = static_cast<ReturnCode::type>(rc);
  }
  return return_code;
}

ReturnCode::type zkclient::drop_listener_with_path(boost::shared_ptr< watcher > listener, string path) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  boost::mutex::scoped_lock lock(background_mutex);
  listener_map::iterator it;
  if ((it = _listeners.find(listener)) != _listeners.end()) {
    listener_map_value::iterator jt;
    if ((jt = it->second.find(path)) != it->second.end()) {
      it->second.erase(jt); 
    }
    return ReturnCode::Ok;
  } else {
    return ReturnCode::Error;
  }
  // never go here
  return ReturnCode::Error;
}

ReturnCode::type zkclient::drop_listener(boost::shared_ptr< watcher > listener) {
  if (!this->is_avaiable()) {
    return ReturnCode::Error;
  }
  boost::mutex::scoped_lock lock(background_mutex);
  listener_map::iterator it;
  if ((it = _listeners.find(listener)) != _listeners.end()) {  //already in map
    if (!it->first->is_live()) {  //removed in past
      return ReturnCode::Ok;
    } else {
      it->first->close();
      return ReturnCode::Ok;
    }
  } else {  //not in map
    return ReturnCode::Error;
  }
  // never go here
  return ReturnCode::Error;
}

}  //namespace cczk
