#include <zkclient.h>

namespace xcs {
  
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

  
  
  
}
