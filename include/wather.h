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
#include <boost/noncopyable.hpp>

namespace cczk {
  using boost::noncopyable;
  class Wathcer : noncopyable {
  public  :
    Wathcer() {
      live = true;
    }
     
  private :
    bool live;
    
  };
} // namespace cczk


#endif
