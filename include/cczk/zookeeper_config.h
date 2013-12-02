/**
 * @file   zookeeper_config.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
 
#ifndef _CCZK_ZOOKEEPER_CONFIG_H_
#define _CCZK_ZOOKEEPER_CONFIG_H_

#include <string>
#include <map>
#include <boost/concept_check.hpp>

#include <gflags/gflags.h>

namespace xcs {
namespace cczk {
DECLARE_string(xcs_conf);
using std::string;
using std::pair;
  
  class zookeeper_config {
  private :
    string host;
    int session_timeout;
    
    string root;
  public  :
    std::map<string, string> auth;
    
    explicit zookeeper_config(const string,const int,const string);
    explicit zookeeper_config();
    
    void load_from_file(const string file_path = FLAGS_xcs_conf);
    
    string get_host();
    void set_host(const string);
    
    int get_session_timeout();
    void set_session_timeout(const int);
    
    string get_root();
    void set_root(const string);
   
    void add_auth(pair<string, string>);
    void remove_auth(string);
    
    bool operator !=(const zookeeper_config &conf) const {
      return !(*this == conf);
    }
    
    bool operator ==(const zookeeper_config &conf) const {
      return (host == conf.host) && \
      (session_timeout == conf.session_timeout) && \
      (root == conf.root);
    }
  };
  
}  // namespace cczk
}  // namespace xcs

#endif