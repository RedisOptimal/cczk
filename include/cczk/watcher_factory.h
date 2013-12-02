/**
 * @file   node_watcher.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
 
#ifndef _CCZK_WATCHER_FACTORY_H_
#define _CCZK_WATCHER_FACTORY_H_
#include <map>

#include <boost/concept_check.hpp>
#include <boost/shared_ptr.hpp>

#include <watcher.h>
#include <zkclient.h>

namespace xcs {
namespace cczk {
  class watcher_factory {
  public  :
    static boost::shared_ptr<watcher> get_watcher(watcher::Listener &listener, bool watch_data, bool watch_child) {
      boost::shared_ptr<watcher> instance(new watcher(listener, watch_data, watch_child));
      return instance;
    }
  };
}  // namespace cczk
}  // namespace xcs


#endif