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

#ifndef _CCZK_ZK_WATHER_H_
#define _CCZK_ZK_WATHER_H_
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

#include <zookeeper_const.h>

namespace cczk {
  using boost::noncopyable;
  class Wathcer : noncopyable {
  public  :
    typedef boost::function<void(const std::string&, WatchEvent::type)> Listener;
    explicit Wathcer(Wathcer::Listener &listener) {
      _live = true;
      _listener = listener;
    }
    
    void close() {
      _live = false;
    }
    
    bool is_live() {
      return _live;
    }
    
  private :
    bool _live;
    Listener _listener;
  };
} // namespace cczk


#endif
