#include <zkclient.h>
#include <zookeeper_config.h>
#include <gtest/gtest.h>


TEST(ZKCLIENT, OPEN) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  config.set_host("no-exist:2181");
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp == NULL);
  
  config.set_host("localhost:2181");
  config.set_root("");
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
}

TEST(ZKCLIENT, CLOSE) {
  using namespace cczk;
  
}

TEST(ZKCLIENT, CLEAR) {
  using namespace cczk;
  
}

TEST(ZKCLIENT, IS_AVALIABLE) {
  using namespace cczk;
  
}

TEST(ZKCLIENT, GET_CHILDREN_OF_PATH) {
  using namespace cczk;
  
  
}

TEST(ZKCLIENT, SET_DATA_OF_NODE) {
  using namespace cczk;
  
  
}

TEST(ZKCLIENT, GET_DATA_OF_NODE) {
  using namespace cczk;
  
}

TEST(ZKCLIENT, CREATE_NODE) {
  using namespace cczk;
  
  
}


TEST(ZKCLIENT, DELETE_NODE) {
  using namespace cczk;
  
  
}

TEST(ZKCLIENT, EXIST) {
  using namespace cczk;
  
}

TEST(ZKCLIENT, ADD_LISTENER) {
  using namespace cczk;
  
  
}


TEST(ZKCLIENT, DROP_LISTENER) {
  using namespace cczk;
  
  
}


TEST(ZKCLIENT, DROP_LISTENER_WITH_PATH) {
  using namespace cczk;
  
  
}