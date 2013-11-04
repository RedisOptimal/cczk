#include <zkclient.h>

namespace cczk {
DEFINE_int32(xcs_zk_node_max_length, 1024*1024, "The default length of zookeeper node's data is 1M.");

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
  }
  sleep(rand() % 10);
  _zhandle = zookeeper_init((_config.get_host()+"/"+_config.get_root()).c_str(),
                            zkclient::init_watcher,
                            3000,
                            NULL,
                            static_cast<void*>(this),
                            0);
  int sleep_interval = 1;
  while (zoo_state(_zhandle) != ZOO_CONNECTED_STATE) {
    usleep(sleep_interval);
    if (sleep_interval > _config.get_session_timeout()) {
      zookeeper_close(_zhandle);
      break;
    }
    sleep_interval <= 1;
  }
  
  return _zhandle;
}

void zkclient::watcher_loop() {
 
}
 
bool zkclient::is_avaiable()  {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
  if (_zhandle != NULL && zoo_state(_zhandle) == SessionState::Connected) {
    return true;
  }
  return false;
}
 
void zkclient::close() {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
  if (_zhandle != NULL) {
    zookeeper_close(_zhandle);
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
    }
  } 
}

void zkclient::event_watcher(zhandle_t* zh, int type, 
      int state, const char* path, void* watcherCtx) {
  
}


zkclient* zkclient::Open(const zookeeper_config config)  {
  static zkclient instance;
  boost::mutex::scoped_lock lock(instance.singleton_mutex);
  if (config != instance._config) {
    instance._config = config;
    if (NULL == instance.create_connection()) {
      return NULL;
    }
    instance.update_auth();
    return &instance;
  }
  if (config == instance._config) {
    instance._config = config;
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
 
ReturnCode::type zkclient::get_children_of_path(string path, std::vector< string >& children) {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
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

ReturnCode::type zkclient::set_data_of_node(string path, string& value) {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
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


ReturnCode::type zkclient::get_data_of_node(string path, string& value) {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
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
 
ReturnCode::type zkclient::exist(string path) {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
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
 
ReturnCode::type zkclient::create_node(string path, string& data, CreateMode::type mode) {
  boost::mutex::scoped_lock lock(this->singleton_mutex);
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
  return ReturnCode::Ok;
}
 
ReturnCode::type zkclient::delete_node(string path) {
  int rc = zoo_delete(_zhandle, path.c_str(), -1);
  if (rc != ZOK) {
    return static_cast<ReturnCode::type>(rc);
  }
  return ReturnCode::Ok;
}
 
}  //namespace cczk
