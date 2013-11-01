/**
 * @file   children_watcher.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
 
#ifndef _CCZK_NDOE_WATCHER_H_
#define _CCZK_NODE_WATCHER_H_
#include <boost/concept_check.hpp>
#include <watcher.h>

namespace cczk {
  class children_watcher : watchcer {
  public  :
    explicit children_watcher(Listener& listener) : wathcer(listener) {
    }
  
  private :
    
    
  };
}  // namespace cczk

#endif