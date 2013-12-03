/**
 * Copyright 2013 Renren.com
 * @file   wathcer.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * This is watcher class header file.
 * 
 */

#ifndef INCLUDE_CCZK_WATCHER_H_
#define INCLUDE_CCZK_WATCHER_H_
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/concept_check.hpp>

#include <zookeeper_const.h>

#include <string>

namespace xcs {
namespace cczk {
using boost::noncopyable;
class Watcher : noncopyable {
 public  :
  typedef boost::function<void(const std::string&, WatchEvent::type)> Listener;
  explicit Watcher(const Listener &listener,
                   bool watch_data,
                   bool watch_child) {
    listener_ = listener;
    watch_data_ = watch_data;
    watch_child_ = watch_child;
  }

  void Close() {
    CloseWatchChild();
    CloseWatchData();
  }

  bool IsLive() {
    return IsWatchData() || IsWatchChild();
  }

  Listener GetListener() {
    return listener_;
  }

  bool IsWatchData() {
    return watch_data_;
  }

  bool IsWatchChild() {
    return watch_child_;
  }

 private :
  bool watch_data_;
  bool watch_child_;
  Listener listener_;

  void CloseWatchData() {
    watch_data_ = false;
  }

  void CloseWatchChild() {
    watch_child_ = false;
  }
};

}  //  namespace cczk
}  //  namespace xcs

#endif  // INCLUDE_CCZK_WATCHER_H_
