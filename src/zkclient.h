/**
 * @file   zkclient.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
 
#ifndef _CCZ_ZKCLIENT_H_
#define _CCZ_ZKCLIENT_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <time.h>

#include <zookeeper/zookeeper.h>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/concept_check.hpp>

#include <zookeeper_config.h>
#include <zookeeper_const.h>
#include <logging.h>

namespace cczk {
  using boost::noncopyable;
  class zkclient : noncopyable {
  private :
    zhandle_t *_zhandle;
    boost::mutex _glob_mutex;
    zookeeper_config _config;
    
    zkclient() {
      srand(getpid());
      _zhandle = NULL;
    }
    
    void update_auth();
    zhandle_t* create_connection();
    
    static void init_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx) {
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

    
    
  public  :
    static zkclient* Open(const zookeeper_config config) {
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
    
    void close() {
      if (_zhandle != NULL) {
        zookeeper_close(_zhandle);
      }
    }
    
    bool is_avaiable() {
      if (_zhandle != NULL && zoo_state(_zhandle) == SessionState::Connected) {
        return true;
      }
      return false;
    }
    
    
    
  };
  
}

#endif