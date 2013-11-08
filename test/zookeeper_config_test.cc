#include <zookeeper_config.h>
#include <gtest/gtest.h>

TEST(ZOOKEEPER_CONFIG, CONSTRUCTION) {
  using namespace cczk;
  zookeeper_config config;
  zookeeper_config config2("host", 3000, "/root");
  zookeeper_config config3("host", 3000, "root");
  ASSERT_EQ(config.get_host(), "");
  ASSERT_EQ(config.get_root(), "");
  ASSERT_EQ(config.get_session_timeout(), 0);
  ASSERT_EQ(config2.get_host(), "host");
  ASSERT_EQ(config2.get_root(), "root");
  ASSERT_EQ(config2.get_session_timeout(), 3000);
  ASSERT_EQ(config3.get_root(), "root");
}

TEST(ZOOKEEPER_CONFIG, LOAD_FROM_FILE) {
  
}