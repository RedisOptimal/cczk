/**
 * Copyright 2013 Renren.com
 */
#include "xcs/cczk/zookeeper_config.h"

#include <stdlib.h>
#include <string.h>

#include "xcs/logging.h"

#include <fstream>
#include <map>

namespace xcs {
namespace cczk {
DEFINE_string(xcs_conf, "./zk.conf", "The path of zk.conf");

ZookeeperConfig::ZookeeperConfig() {
  set_host("");
  set_session_timeout(0);
  set_root("");
  auth.clear();
}

ZookeeperConfig::ZookeeperConfig(const string host,
                                 const int session_timeout,
                                 const string root) {
  set_host(host);
  set_session_timeout(session_timeout);
  set_root(root);
  auth.clear();
}

static string Trim(string str) {
  int i, j;
  for (i = 0; i < (int)str.size(); i++) {
    if (str[i] != ' ' && str[i] != '\t')
      break;
  }
  for (j = (int)str.size() - 1; j >= 0; j--) {
    if (str[j] != ' ' && str[j] != '\t')
      break;
  }
  if (i > j)  return "";
  else  return str.substr(i, j - i + 1);
}

void ZookeeperConfig::LoadFromFile(const string file_path) {
  std::ifstream cfg(file_path.c_str());
  if (!cfg.is_open()) {
    XCS_FATAL << "XCS: Failed to open property configure file:"
        << file_path;
  }
  string cfg_line;
  std::map<string, string> props;
  while (getline(cfg, cfg_line)) {
    if (cfg_line.size() < 1 || cfg_line[0] == '#')
      continue;
    int index = cfg_line.find("=");
    if (index == -1)
      continue;
    string key = cfg_line.substr(0, index);
    string value = cfg_line.substr(index + 1);
    props.insert(std::pair<string, string>(Trim(key), Trim(value)));
  }
  std::map<string, string>::iterator it;
  it = props.find("root");
  if (it != props.end()) {
    set_root(it->second);
  }
  it = props.find("cluster");
  if (it != props.end()) {
    set_host(it->second);
  }
  it = props.find("session_timeout");
  if (it != props.end()) {
    set_session_timeout(atoi(it->second.c_str()));
  }
  it = props.find("auth");
  if (it != props.end()) {
    string auth_str = it->second;
    do {
      pair<string, string> auth_pair;
      string::size_type pos = auth_str.find(",");
      string one_auth;
      if (pos != string::npos) {
        one_auth = auth_str.substr(0, pos);
        auth_str = auth_str.substr(pos+1);
      } else {
        one_auth = auth_str;
      }
      
      pos = one_auth.find(":");
      if (pos != string::npos) {
        auth_pair = std::make_pair(one_auth.substr(0, pos), 
                                   one_auth.substr(pos + 1));
        if (!auth_pair.first.empty() && !auth_pair.second.empty()) {
          add_auth(auth_pair);
        }
      }
    } while (auth_str.empty());    
  }
}

string ZookeeperConfig::get_host() {
  return host_;
}

void ZookeeperConfig::set_host(const string host) {
  this->host_ = host;
}

int ZookeeperConfig::get_session_timeout() {
  return session_timeout_;
}

void ZookeeperConfig::set_session_timeout(const int session_timeout) {
  this->session_timeout_ = session_timeout;
}

string ZookeeperConfig::get_root() {
  return root_;
}

void ZookeeperConfig::set_root(const string root) {
  this->root_ = root;
  if (this->root_.find_first_of("/") == 0) {
    this->root_ = this->root_.substr(1);
  }
}

void ZookeeperConfig::add_auth(pair<string, string> auth_pair) {
  auth.insert(auth_pair);
}

void ZookeeperConfig::remove_auth(string username) {
  if (auth.find(username) != auth.end()) {
    auth.erase(username);
  } 
}

}  // namespace cczk
}  // namespace xcs
