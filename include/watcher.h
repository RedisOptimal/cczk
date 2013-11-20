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

#include <zookeeper_const.h>

namespace cczk {
  using boost::noncopyable;
  class watcher : noncopyable {
  public  :
    typedef boost::function<void(const std::string&, WatchEvent::type)> Listener;

    bool _watch_data;
    bool _watch_children;

    explicit watcher(Listener &listener) {
      _watch_data = true;
      _watch_children = false;
      _live = true;
      _listener = listener;
    }
    
    void close() {
      _live = false;
    }
    
    bool is_live() {
      return _live;
    }
    
    void set_listener(Listener &listener) {
      _listener = listener;
    }
    
    Listener get_listener() {
      return _listener;
    }
  private :
    bool _live;
    Listener _listener;
  };
} // namespace cczk


#endif
