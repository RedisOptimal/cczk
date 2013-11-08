#include <zkclient.h>
#include <zookeeper_config.h>
#include <gtest/gtest.h>


TEST(ZKCLIENT, OPEN) {
  using namespace cczk;
  zookeeper_config config("10.11.21.185:21810", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  config.set_host("no-exist:2181");
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp == NULL);
  
  config.set_host("10.11.21.185:21810");
  config.set_root("");
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
}
