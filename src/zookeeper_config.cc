#include <zookeeper_config.h>

#include <string.h>

#include <map>
#include <fstream>

#include <logging.h>

namespace cczk {
DEFINE_string(xcs_conf, "./zk.conf", "The path of zk.conf");

zookeeper_config::zookeeper_config() {
  set_host("");
  set_session_timeout(0);
  set_root("");
  auth.clear();
}

zookeeper_config::zookeeper_config(const string host,
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

void zookeeper_config::load_from_file(const string file_path) {
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

string zookeeper_config::get_host() {
  return host;
}

void zookeeper_config::set_host(const string host) {
  this->host = host;
}

int zookeeper_config::get_session_timeout() {
  return session_timeout;
}

void zookeeper_config::set_session_timeout(const int session_timeout) {
  this->session_timeout = session_timeout;
}

string zookeeper_config::get_root() {
  return root;
}

void zookeeper_config::set_root(const string root) {
  this->root = root;
  if (this->root.find_first_of("/") == 0) {
    this->root = this->root.substr(1);
  }
}

void zookeeper_config::add_auth(pair<string, string> auth_pair) {
  auth.insert(auth_pair);
}

void zookeeper_config::remove_auth(string username) {
  if (auth.find(username) != auth.end()) {
    auth.erase(username);
  } 
}

}  //namespace cczk