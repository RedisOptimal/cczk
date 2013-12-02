#include "service_registry_accessor.h"
#include "logging.h"

#include <string>
#include <vector>

namespace xcs {
/*
ServiceRegistryAccessor::ServiceRegistryAccessor(const std::string& serivceId,
                                                 const std::string& version,
                                                 const std::string& stat) :
    service_(serivceId), version_(version), stat_(stat)
{
  service_path_ = "/" + service_ + "/" + version_ + "/" + stat_;
}

ServiceRegistryAccessor::~ServiceRegistryAccessor() {
  std::set<Listener*>::iterator set_it = listener_set_.begin();
  ZooKeeperX* instance = ZooKeeperX::Instance();
  for (; set_it != listener_set_.end(); ++set_it) {
    instance->DropListener(*set_it);
  }
}

int
ServiceRegistryAccessor::ListAndListen(std::vector<std::string>& children,
                                       Listener* listener) {
  ZooKeeperX* instance = ZooKeeperX::Instance();

  if (listener != NULL) {
    listener_set_.insert(listener);
    if (instance->AddChildrenListener(service_path_, listener) !=
        ReturnCode::Ok) {
      return -1;
    }
  }

  if (instance->GetChildren(service_path_, children) != ReturnCode::Ok) {
    return -1;
  }
  
  return 0;
}

int
ServiceRegistryAccessor::ContentListen(Listener* listener) {
  if (listener == NULL) {
    XCS_ERROR << "Null listener error!\n";
    return -1;
  }

  listener_set_.insert(listener);
  
  std::vector<std::string> children;
  if (ZooKeeperX::Instance()->GetChildren(service_path_,children) !=
      ReturnCode::Ok) {
    return -1;
  }
  
  std::vector<std::string>::iterator it = children.begin();
  for (; it != children.end(); ++it) {
    std::string endpoint = service_path_ + "/" + (*it);
    ZooKeeperX::Instance()->AddNodeListener(endpoint, listener);
  }

  return 0;
}

int
ServiceRegistryAccessor::GetServiceStatus(const std::string &endpoint,
                                          std::string& content,
                                          Stat& nodeStat)
{
  std::string endpoint_path = service_path_ + "/" + endpoint;
  if (ZooKeeperX::Instance()->Get(endpoint_path, content, nodeStat) !=
      ReturnCode::Ok) {
    return -1;
  }

  return 0;
}*/
}
