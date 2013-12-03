#include <gtest/gtest.h>
#include <service_registry.h>
#include <cczk/zookeeper_config.h>
#include <cczk/zkclient.h>

TEST(SERVICE_REGISTRY, SERVICE_NODE) {
  xcs::ServiceNode node1;
  node1.name_ = "127.0.0.1:80";
  node1.content_ = "test";
  
  ASSERT_TRUE("127.0.0.1:80" == node1.name_);
  ASSERT_TRUE("test" == node1.content_);
}

TEST(SERVICE_REGISTRY, SERVICE_REGISTRY) {
  using namespace xcs;
  ServiceNode node1;
  node1.name_ = "127.0.0.1:2181";
  node1.content_ = "test";
  
  int rc;
  ServiceRegistry *registry = new ServiceRegistry();
  std::string serviceid = "test.service";
  std::string version = "1";
  std::string stat = "0";
  rc = registry->PublishService(serviceid, version, stat, node1);
  ASSERT_TRUE(rc == -2);
  
  cczk::ZookeeperConfig config;
  config.set_host("xcszookeepertest.n.xiaonei.com:2181");
  config.set_root("/xcs-test");
  config.set_session_timeout(4000);
  cczk::ZkClient *tmp = cczk::ZkClient::Open(&config);
  
  rc = registry->PublishService(serviceid, version, stat, node1);
  ASSERT_TRUE(rc == 0);
  //waiting register
  sleep(5);
  std::string path = "/" + serviceid + "/" + version + "/" + stat + "/" + node1.name_;
  tmp->DeleteNode(path);
  //waiting watcher register back
  sleep(20);
  ReturnCode::type ret;
  ret = tmp->Exist(path);
  ASSERT_TRUE(ret == ReturnCode::Ok);
  
  delete registry;
  sleep(5);
  ret = tmp->Exist(path);
  ASSERT_TRUE(ret != ReturnCode::Ok);
}



