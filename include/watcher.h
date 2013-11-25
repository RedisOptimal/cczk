/**
 * @file   wathcer.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */

#ifndef _CCZK_ZK_WATCHER_H_
#define _CCZK_ZK_WATCHER_H_
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/concept_check.hpp>

#include <zookeeper_const.h>

namespace cczk {
  using boost::noncopyable;
  class watcher : noncopyable {
  public  :
    typedef boost::function<void(const std::string&, WatchEvent::type)> Listener;
    explicit watcher(Listener &listener, bool watch_data, bool watch_child) {
      _listener = listener;
      _watch_data = watch_data;
      _watch_child = watch_child;
    }
    
    void close() {
      close_watch_child();
      close_watch_data();
    }
    
    bool is_live() {
      return watch_data() || watch_child();
    }
    
    Listener get_listener() {
      return _listener;
    }
    
    bool watch_data() {
      return _watch_data; 
    }
    
    bool watch_child() {
      return _watch_child; 
    }
    
  private :
    bool _watch_data;
    bool _watch_child;
    Listener _listener;
    
    void close_watch_data() {
      _watch_data = false; 
    }
    
    void close_watch_child() {
      _watch_child = false; 
    }
  };
} // namespace cczk


#endif
