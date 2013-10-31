#ifndef _XCS_ZK_WATHER_H_
#define _XCS_ZK_WATHER_H_
#include <boost/noncopyable.hpp>

namespace xcs {
  using boost::noncopyable;
  class Wathcer : noncopyable {
  public  :
    Wathcer() {
      live = true;
    }
     
  private :
    bool live;
    
  };
} // namespace xcs


#endif
