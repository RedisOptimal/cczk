#include "xcs/logging.h"
#include "xcs/service_registry.h"
#include "xcs/cczk/watcher_factory.h"

namespace xcs {

using std::string;

DEFINE_int32(xcs_holder_time, 300,
             "Default publisher holder time to check ephemeral node time");

ServiceRegistry::ServiceRegistry() {
  Watcher::Listener tmp_listener = boost::bind(&ServiceRegistry::ContentChangeListener, this, _1, _2);
  listener_ = WatcherFactory::get_watcher(tmp_listener, true, false);
}

ServiceRegistry::~ServiceRegistry() {
/*  holder_thread_running_ = false;
  thread_.join();
*/
  ZkClient *instance = ZkClient::Open();
  
  instance->DropListener(listener_);
  
  std::map<string, std::pair<bool, string> >::iterator map_it;
  boost::mutex::scoped_lock lock(mutex_);
  for (map_it = nodes_.begin(); map_it != nodes_.end(); ++map_it) {
    instance->DeleteNode(map_it->first);
  }
}

int ServiceRegistry::PublishService(const string& service,
                                    const string& version,
                                    const string& stat,
                                    const ServiceNode& node,
                                    bool is_tmp) {
  if (service.empty() || version.empty() || stat.empty() || node.name_.empty()) {
    XCS_ERROR << "ServiceRegistry::PublishService() error: [service: " << service
              << "; version: " << version << "; stat: " << stat << "; node:"
              << node.name_ << "]\n";
    return -1;
  }
  
  ZkClient *instance = ZkClient::Open();
  
  string path = "/" + service + "/" + version + "/" + stat + "/" + node.name_;

  ReturnCode::type zoo_rc = instance->CreateNode(path,
                                                  node.content_,
                                                  is_tmp ? CreateMode::Ephemeral : CreateMode::Persistent);
  if (zoo_rc != ReturnCode::Ok) {
    return -2;
  }

  if (is_tmp) {                         
    {
      boost::mutex::scoped_lock lock(mutex_);
      nodes_[path] = std::make_pair(is_tmp, node.content_);
    }
    // Idle for 100ms to make sure the node is created
    usleep(100000);
    
    zoo_rc = instance->AddListener(listener_, path);
    if (zoo_rc != ReturnCode::Ok) {
      XCS_ERROR << "[PublishService]" << ReturnCode::toString(zoo_rc) << "@PATH=" << path;
      return -3;
    }
  }

  return 0;
}


int ServiceRegistry::PublishService(const string& prefix,
                                    const ServiceNode& node,
                                    bool is_tmp) {
  if (prefix.empty() || node.name_.empty()) {
    XCS_ERROR << "ServiceRegistry::PublishService() error: [Path prefix : "
              << prefix <<  "; node:" << node.name_ << "]\n";
    return -1;
  }
  ZkClient *instance = ZkClient::Open();
  
  xcs::ReturnCode::type ret;
  ret = instance->Exist(prefix);
  if (ret != xcs::ReturnCode::Ok) {
    XCS_ERROR << "ServiceRegistry::PublishService() error: [prefix = "
    << prefix << " not exist on zk, take care of `ZookeeperConfig.root_`]";
    return -1;
  }
  
  string path = "/" + prefix + "/" + node.name_;

  ReturnCode::type zoo_rc = instance->CreateNode(path,
                                                 node.content_,
                                                 is_tmp ? CreateMode::Ephemeral : CreateMode::Persistent);
  if (zoo_rc != ReturnCode::Ok) {
    return -2;
  }

  if (is_tmp) {                         
    {
      boost::mutex::scoped_lock lock(mutex_);
      nodes_[path] = std::make_pair(is_tmp, node.content_);
    }
    // Idle for 100ms to make sure the node is created
    usleep(100000);
    
    zoo_rc = instance->AddListener(listener_, path);
    if (zoo_rc != ReturnCode::Ok) {
      XCS_ERROR << "[PublishService]" << ReturnCode::toString(zoo_rc) << "@PATH=" << path;
      return -3;
    }
  }

  return 0;
}

void ServiceRegistry::ContentChangeListener(const string& path,
                                            WatchEvent::type type)
{
  ZkClient *instance = ZkClient::Open();

  if (type == WatchEvent::ZnodeDataChanged) {
    boost::mutex::scoped_lock lock(mutex_);
    if (nodes_.find(path) != nodes_.end()) {
      string old_value = nodes_.find(path)->second.second;
      string zk_value;
      ReturnCode::type rc = instance->GetDataOfNode(path, zk_value);
      if (rc != ReturnCode::Ok) {
        XCS_ERROR << "ContentChangeListener() failed: can not get node\n";
        rc = instance->Exist(path);
        if (rc != ReturnCode::Ok) {
          XCS_ERROR << "Node not exist @PATH=" << path;
          bool is_tmp = nodes_.find(path)->second.first;
          rc = instance->CreateNode(path, old_value, is_tmp ? CreateMode::Ephemeral : CreateMode::Persistent);
          if (rc != ReturnCode::Ok) {
            XCS_ERROR << "Create Node Failed"; 
          }
          rc = instance->AddListener(listener_, path);
          if (rc != ReturnCode::Ok) {
            XCS_ERROR << "Register listener back failed."; 
          }
        }
      } else if (zk_value != old_value) {
        XCS_INFO << "Content change from [" << old_value << "] to [" << zk_value
                 << "]\n";
        nodes_[path].second = zk_value;
      }
    } else {
      XCS_ERROR << "Content event triggered on path [" << path
                << "] which is not exist in registry.\n";
    }
  } else {
    XCS_ERROR << "Suspicious event type [" << WatchEvent::toString(type)
              << "] triggered on path [" << path << "]\n";
  }
}

}
