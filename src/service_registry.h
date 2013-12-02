/**
 * @file   service_registry.h
 * @author dinggangyilang@gmail.com
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @date   2012-08-28 created
 * @date   Wed Sep 18 16:21:52 2013 reconstructed
 * 
 * @brief 帮助XCS的server角色发布服务
 *
 */

#ifndef _XCS_SERVICE_REGISTRY_H_
#define _XCS_SERVICE_REGISTRY_H_

#include "cczk/zkclient.h"

#include <string>
#include <vector>

namespace xcs {

using namespace cczk;
  
struct ServiceNode {
  std::string name_;     // 服务的地址(ip:port)
  std::string content_;  // 服务节点存储的value
};

class ServiceRegistry {
 public:
  explicit ServiceRegistry();
  
  ~ServiceRegistry();

/**
  * 发布服务操作。注意前三项是由OP角色建立的路径，不会在程序中建立
  * 
  * @param service 服务的唯一名称
  * @param version 特定的版本号，如 1/2/3
  * @param stat    服务状态分区名(sharding)
  * @param node    发布的服务地址及其服务状态（value值）
  * @param is_tmp  标示发布的服务是临时的还是永久的，默认是临时的
  * return         0: 成功
  *               -1: 失败，严重问题，参数不全
  *               -2: 失败，但是发布节点已经加入了后台线程，
  *                         会在随后Zookeeper连接可用时恢复
  */
  int PublishService(const std::string& service,
                     const std::string& version,
                     const std::string& stat,
                     const ServiceNode& node,
                     bool is_tmp = true);
 private:
  /**
   * 节点内容变动的监听
   *
   * 节点的 Enable/Disable 状态是由OP角色触发的，这里进行监听，修改内部记录的
   * value 值，从而在临时节点重新注册的时候放置正确的内容
   *
   * @param path
   * @param type
   */
  void ContentChangeListener(const std::string& path, WatchEvent::type type);

  /**
   * 对于临时节点，可能在超时之后消失，需要内部线程进行验证和追加注册
   */
  void Holder();

 private:
  ///< Map(Path, Value)
  std::map<std::string, std::pair<bool, std::string> > nodes_;
  boost::mutex mutex_;

  boost::shared_ptr<watcher> listener_;

};

}

#endif  // XCS_SERVICE_REGISTRY_H_
