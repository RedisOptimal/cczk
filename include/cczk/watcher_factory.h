/**
 * Copyright 2013 Renren.com
 * @file   node_watcher.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
#ifndef INCLUDE_CCZK_WATCHER_FACTORY_H_
#define INCLUDE_CCZK_WATCHER_FACTORY_H_
#include <boost/concept_check.hpp>
#include <boost/shared_ptr.hpp>

#include <watcher.h>
#include <zkclient.h>

#include <map>

namespace xcs {
namespace cczk {
class WatcherFactory {
 public  :
  static boost::shared_ptr<Watcher> get_watcher(const Watcher::Listener &listener,
                                                bool watch_data,
                                                bool watch_child) {
    boost::shared_ptr<Watcher> instance(new Watcher(listener,
                                                    watch_data,
                                                    watch_child));
    return instance;
  }
};

}  // namespace cczk
}  // namespace xcs


#endif  // INCLUDE_CCZK_WATCHER_FACTORY_H_