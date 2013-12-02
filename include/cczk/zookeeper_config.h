/**
 * Copyright 2013 Renren.com
 * @file   zookeeper_config.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
#ifndef INCLUDE_CCZK_ZOOKEEPER_CONFIG_H_
#define INCLUDE_CCZK_ZOOKEEPER_CONFIG_H_
#include <gflags/gflags.h>

#include <string>
#include <map>
#include <utility>

namespace xcs {
namespace cczk {
DECLARE_string(xcs_conf);
using std::string;
using std::pair;

class ZookeeperConfig {
 private :
  string host_;
  int session_timeout_;

  string root_;
 public  :
  std::map<string, string> auth;

  explicit ZookeeperConfig(const string, const int, const string);
  explicit ZookeeperConfig();

  void LoadFromFile(const string file_path = FLAGS_xcs_conf);

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

#endif  // INCLUDE_CCZK_ZOOKEEPER_CONFIG_H_