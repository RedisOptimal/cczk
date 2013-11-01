#include <zkclient.h>

namespace cczk {
  
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
    if (_zhandle != NULL && zoo_state(_zhandle) == SessionState::Connected) {
      return true;
    }
    return false;
  }

  void zkclient::close() {
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
          //TO-DO lOG   
        }
      } else if (state == SessionState::Connecting) {
          //TO-DO lOG   
      }
    } 
  }

  zkclient* zkclient::Open(const zookeeper_config config)  {
    static zkclient instance;
    static boost::mutex singleton_mutex;
    boost::mutex::scoped_lock lock(singleton_mutex);
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
 
}  //namespace cczk
