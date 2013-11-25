#include "zookeeper_const.h"
#include "logging.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <iostream>

namespace cczk {

namespace ReturnCode {

const std::string toString(type rc) {
  switch (rc) {
    case Ok:
      return "Ok";
    case SystemError:
      return "SystemError";
    case RuntimeInconsistency:
      return "RuntimeInconsistency";
    case DataInconsistency:
      return "DataInconsistency";
    case ConnectionLoss:
      return "ConnectionLoss";
    case MarshallingError:
      return "MarshallingError";
    case Unimplemented:
      return "Unimplemented";
    case OperationTimeout:
      return "OperationTimeout";
    case BadArguments:
      return "BadArguments";
    case ApiError:
      return "ApiError";
    case NoNode:
      return "NoNode";
    case NoAuth:
      return "NoAuth";
    case BadVersion:
      return "BadVersion";
    case NoChildrenForEphemerals:
      return "NoChildrenForEphemerals";
    case NodeExists:
      return "NodeExists";
    case NotEmpty:
      return "NotEmpty";
    case SessionExpired:
      return "SessionExpired";
    case InvalidCallback:
      return "InvalidCallback";
    case InvalidAcl:
      return "InvalidAcl";
    case AuthFailed:
      return "AuthFailed";
    case SessionMoved:
      return "SessionMoved";
    case CppError:
      return "CppError";
    case InvalidState:
      return "InvalidState";
    case Error:
      return "Error";
    case ZkClosing:
      return "ZookeeperClosing";
    case ZkNothing:
      return "ZookeeperNothing";
  }
  return str(boost::format("UnknownError(%d)") % rc);
}
}  // namespace ReturnCode

namespace SessionState {

const std::string toString(type state) {
  switch (state) {
    case Expired:
      return "Expired";
    case AuthFailed:
      return "AuthFailed";
    case Connecting:
      return "Connecting";
    case Connected:
      return "Connected";
  }
  XCS_ERROR << "Unknown session state: " << state << std::endl;
  return str(boost::format("UnknownSessionState(%d)") % state);
}

}  // namespace SessionState

namespace WatchEvent {

const std::string toString(type eventType) {
  switch (eventType) {
    case SessionStateChanged:
      return "SessionStateChanged";
    case ZnodeCreated:
      return "ZnodeCreated";
    case ZnodeRemoved:
      return "ZnodeRemoved";
    case ZnodeDataChanged:
      return "ZnodeDataChanged";
    case ZnodeChildrenChanged:
      return "ZnodeChildrenChanged";
  }
  XCS_ERROR << "Unknown watch event: " << eventType << std::endl;
  return str(boost::format("UnknownWatchEvent(%d)") % eventType);
}

}  // namespace WatchEvent

namespace CreateMode {
const std::string toString(int32_t createMode) {
  switch (createMode) {
    case Persistent:
      return "Persistent";
    case PersistentSequential:
      return "PersistentSequential";
    case Ephemeral:
      return "Ephemeral";
    case EphemeralSequential:
      return "EphemeralSequential";
  }
  XCS_ERROR << "Unknow create mode: " << createMode << std::endl;
  return str(boost::format("UnknowCreateMode(%d)") % createMode);
}
  
}  // namespace CreateMode

namespace Permission {

const std::string toString(int32_t permType) {
  std::vector<std::string> permissions;
  if (permType & Read) {
    permissions.push_back("Read");
  }
  if (permType & Write) {
    permissions.push_back("Write");
  }
  if (permType & Create) {
    permissions.push_back("Create");
  }
  if (permType & Delete) {
    permissions.push_back("Delete");
  }
  if (permType & Admin) {
    permissions.push_back("Admin");
  }
  return boost::algorithm::join(permissions, " | ");
}

}

} // namespace cczk
