/**
 * @file   service_registry.h
 * @author dinggangyilang@gmail.com
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @date 2012-08-28 created
 * @date Wed Sep 18 18:21:34 2013 reconstructed
 * 
 * @brief 帮助XCS的client角色获取服务
 *        注意：一个ServiceRegistryAccessor实例只能对应的获取一个服务版本
 *             若是想获取多个服务版本请创建多个实例
 */
#ifndef XCS_SERVICE_REGISTRY_ACCESSOR_H_
#define XCS_SERVICE_REGISTRY_ACCESSOR_H_

#include "zkclient.h"

#include <vector>
#include <string>

namespace xcs {
  
using namespace cczk;
  
class ServiceRegistryAccessor {
 public:
  /** 
   * @param serivceId 服务的唯一名称
   * @param version 版本号，默认为 1
   * @param stat 分区号，默认为 0
   * 
   */ 
  explicit ServiceRegistryAccessor(const std::string& serivceId,
                                   const std::string& version = "1",
                                   const std::string& stat = "0");

  ~ServiceRegistryAccessor();
  
  /**
   * 获取stat层的子节点列表，可以选择是否对 stat的子节点变动 监听
   * 
   * @param[out] children 输出参数. 保存子节点列表
   * @param listener 监听类. 若为空（NULL），则不进行监听
   *                 
   * return    0为成功
   */ 
  int ListAndListen(std::vector<std::string>& children, boost::shared_ptr<watcher> listener);

  /**
   * 给stat下面的所有子节点的 内容变化 加监听
   *
   * @param listener 监听类，不能为空（NULL)
   * return  0为成功
   */ 
  int ContentListen(boost::shared_ptr<watcher> listener);

  /**
   * 获取某一个服务状态
   *
   * @param endpoint 服务地址(ip:port)，不能为空("")
   * @param[out] content 输出参数，存储服务状态信息
   * return  0为成功
   */
  int GetServiceStatus(const std::string& endpoint, std::string& content);

  // 获取 service
  std::string service() {
    return service_;
  }

  // 获取 version
  std::string version() {
    return version_;
  }

  // 获取 stat
  std::string stat() {
    return stat_;
  }

 private:
  std::string service_;
  std::string version_;
  std::string stat_;

  std::string service_path_;
  std::set<boost::shared_ptr<watcher> > listener_set_;
};

}

#endif  // XCS_SERVICE_REGISTRY_ACCESSOR_H_
