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
  std::set<boost::shared_ptr<watcher> >::iterator set_it = listener_set_.begin();
  zkclient *instance = zkclient::open();
  for (; set_it != listener_set_.end(); ++set_it) {
    instance->drop_listener(*set_it);
  }
}

int
ServiceRegistryAccessor::ListAndListen(std::vector<std::string>& children,
                                       boost::shared_ptr<watcher> listener) {
  zkclient *instance = zkclient::open();
  if (listener != NULL) {
    listener_set_.insert(listener);
    if (instance->add_listener(listener, service_path_) !=
        ReturnCode::Ok) {
      return -1;
    }
  }

  if (instance->get_children_of_path(service_path_, children) != ReturnCode::Ok) {
    return -1;
  }
  
  return 0;
}

int
ServiceRegistryAccessor::ContentListen(boost::shared_ptr<watcher> listener) {
  if (listener == NULL) {
    XCS_ERROR << "Null listener error!\n";
    return -1;
  }

  listener_set_.insert(listener);
  
  std::vector<std::string> children;
  if (zkclient::open()->get_children_of_path(service_path_,children) !=
      ReturnCode::Ok) {
    return -1;
  }
  
  std::vector<std::string>::iterator it = children.begin();
  for (; it != children.end(); ++it) {
    std::string endpoint = service_path_ + "/" + (*it);
    zkclient::open()->add_listener(listener, endpoint);
  }

  return 0;
}

int
ServiceRegistryAccessor::GetServiceStatus(const std::string &endpoint,
                                          std::string& content)
{
  std::string endpoint_path = service_path_ + "/" + endpoint;
  if (zkclient::open()->get_data_of_node(endpoint_path, content) !=
      ReturnCode::Ok) {
    return -1;
  }

  return 0;
}
}
