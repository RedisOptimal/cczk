#include "service_registry_accessor.h"
#include "logging.h"

#include <string>
#include <vector>

namespace xcs {

ServiceRegistryAccessor::ServiceRegistryAccessor(const std::string& serivceId,
                                                 const std::string& version,
                                                 const std::string& stat) :
    service_(serivceId), version_(version), stat_(stat)
{
  service_path_ = "/" + service_ + "/" + version_ + "/" + stat_;
}

ServiceRegistryAccessor::~ServiceRegistryAccessor() {
  std::set<boost::shared_ptr<Watcher> >::iterator set_it = listener_set_.begin();
  ZkClient *instance = ZkClient::Open();
  for (; set_it != listener_set_.end(); ++set_it) {
    instance->DropListener(*set_it);
  }
}

int
ServiceRegistryAccessor::ListAndListen(std::vector<std::string>& children,
                                       boost::shared_ptr<Watcher> listener) {
  ZkClient *instance = ZkClient::Open();
  if (listener != NULL) {
    listener_set_.insert(listener);
    if (instance->AddListener(listener, service_path_) !=
        ReturnCode::Ok) {
      return -1;
    }
  }

  if (instance->GetChildrenOfPath(service_path_, children) != ReturnCode::Ok) {
    return -1;
  }
  
  return 0;
}

int
ServiceRegistryAccessor::ContentListen(boost::shared_ptr<Watcher> listener) {
  if (listener == NULL) {
    XCS_ERROR << "Null listener error!\n";
    return -1;
  }

  listener_set_.insert(listener);
  
  std::vector<std::string> children;
  if (ZkClient::Open()->GetChildrenOfPath(service_path_,children) !=
      ReturnCode::Ok) {
    return -1;
  }
  
  std::vector<std::string>::iterator it = children.begin();
  for (; it != children.end(); ++it) {
    std::string endpoint = service_path_ + "/" + (*it);
    ZkClient::Open()->AddListener(listener, endpoint);
  }

  return 0;
}

int
ServiceRegistryAccessor::GetServiceStatus(const std::string &endpoint,
                                          std::string& content)
{
  std::string endpoint_path = service_path_ + "/" + endpoint;
  if (ZkClient::Open()->GetDataOfNode(endpoint_path, content) !=
      ReturnCode::Ok) {
    return -1;
  }

  return 0;
}

}
