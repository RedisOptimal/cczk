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
 
#ifndef _CCZK_ZKCLIENT_H_
#define _CCZK_ZKCLIENT_H_

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
#include <boost/shared_ptr.hpp>

#include <zookeeper_config.h>
#include <zookeeper_const.h>
#include <logging.h>
#include <watcher.h>

namespace cczk {
  using boost::noncopyable;
  class watcher;
  class zkclient : noncopyable {
  private :
    typedef boost::shared_ptr<watcher> listener_map_key;  // watcher
    typedef std::set<string> listener_map_value;  // path of set
    typedef std::map<listener_map_key, listener_map_value> listener_map;  
             
    zhandle_t *_zhandle;
    boost::mutex _glob_mutex;
    zookeeper_config _config;
    boost::thread _background_watcher_thread;
    bool _background_watcher;
    boost::recursive_mutex background_mutex;
    listener_map _listeners;                
    std::map<string, std::pair<string, CreateMode::type> > _ephemeral_node;
    boost::mutex singleton_mutex;
    
    zkclient();
    
    void update_auth();
    zhandle_t* create_connection();
    
    static void init_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx);

    static void event_watcher(zhandle_t *zh, int type,
        int state, const char *path,void *watcherCtx);
    
    void watcher_loop();
    
    ~zkclient();
    
  public  :
    static zkclient* open(const zookeeper_config*/*config*/);
    
    void close();
    
    void clear();
    
    bool is_avaiable();
   
    ReturnCode::type get_children_of_path(const string/*path*/, std::vector<string>&/*children*/);
    
    ReturnCode::type set_data_of_node(const string/*path*/, string&/*value*/);
    
    ReturnCode::type get_data_of_node(const string/*path*/, string&/*value*/);
    
    ReturnCode::type create_node(const string/*path*/, string&/*value*/, CreateMode::type/*mode*/);
    
    ReturnCode::type delete_node(const string/*path*/);
    
    ReturnCode::type exist(const string/*path*/);
    
    ReturnCode::type add_listener(boost::shared_ptr<watcher>/*listener*/, string/*path*/);
   
    ReturnCode::type drop_listener(boost::shared_ptr<watcher>/*listener*/);
    
    ReturnCode::type drop_listener_with_path(boost::shared_ptr<watcher>/*listener*/, string/*path*/);
  };
  
}

#endif