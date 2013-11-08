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
  
}

TEST(ZKCLIENT, CLEAR) {
  
}

TEST(ZKCLIENT, IS_AVALIABLE) {
  
}

TEST(ZKCLIENT, GET_CHILDREN_OF_PATH) {
  
  
}

TEST(ZKCLIENT, SET_DATA_OF_NODE) {
  
  
}

TEST(ZKCLIENT, GET_DATA_OF_NODE) {
  
}

TEST(ZKCLIENT, CREATE_NODE) {
  
  
}


TEST(ZKCLIENT, DELETE_NODE) {
  
  
}

TEST(ZKCLIENT, EXIST) {
  
}

TEST(ZKCLIENT, ADD_LISTENER) {
  
  
}


TEST(ZKCLIENT, DROP_LISTENER) {
  
  
}


TEST(ZKCLIENT, DROP_LISTENER_WITH_PATH) {
  
  
}