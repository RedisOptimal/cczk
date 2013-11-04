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
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

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
    boost::thread _background_watcher_thread;
    bool _background_watcher;
    boost::mutex singleton_mutex;
    
    zkclient(): _zhandle(NULL),
                _background_watcher(true),
                _background_watcher_thread(boost::bind(&zkclient::watcher_loop, this)) {
      srand(getpid());
    }
    
    void update_auth();
    zhandle_t* create_connection();
    
    static void init_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx);

    static void event_watcher(zhandle_t *zh, int type,
        int state, const char *path,void *watcherCtx);
    
  public  :
    static zkclient* Open(const zookeeper_config/*config*/);
    
    void close();
    
    bool is_avaiable();
    
    void watcher_loop();
   
    ReturnCode::type get_children_of_path(string/*path*/, std::vector<string>&/*children*/);
    
    ReturnCode::type set_data_of_node(string/*path*/, string&/*value*/);
    
    ReturnCode::type get_data_of_node(string/*path*/, string&/*value*/);
    
    ReturnCode::type create_node(string/*path*/, string&/*value*/, CreateMode::type/*mode*/);
    
    ReturnCode::type delete_node(string/*path*/);
    
    ReturnCode::type exist(string/*path*/);
  };
  
}

#endif