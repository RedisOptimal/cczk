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
  std::string version = "0";
  std::string stat = "1";
  rc = registry->PublishService(serviceid, version, stat, node1);
  ASSERT_TRUE(rc == -2);
  
  cczk::zookeeper_config config;
  config.set_host("localhost:2181");
  config.set_root("/test");
  config.set_session_timeout(4000);
  cczk::zkclient *tmp = cczk::zkclient::open(&config);
  
  rc = registry->PublishService(serviceid, version, stat, node1);
  ASSERT_TRUE(rc == 0);
  //waiting register
  sleep(5);
  std::string path = "/" + serviceid + "/" + version + "/" + stat + "/" + node1.name_;
  tmp->delete_node(path);
  //waiting watcher register back
  sleep(20);
  ReturnCode::type ret;
  ret = tmp->exist(path);
  ASSERT_TRUE(ret == ReturnCode::Ok);
  
  delete registry;
  sleep(5);
  ret = tmp->exist(path);
  ASSERT_TRUE(ret != ReturnCode::Ok);
}


