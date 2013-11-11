#include <zkclient.h>
#include <zookeeper_config.h>
#include <gtest/gtest.h>


TEST(ZKCLIENT, OPEN) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  config.set_host("no-exist:2181");
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp == NULL);
  
  config.set_host("localhost:2181");
  config.set_root("");
  tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
  ASSERT_FALSE(tmp->is_avaiable());
  ASSERT_EQ(tmp, zkclient::open(NULL));
  tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  config.set_session_timeout(2000);
  tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
}

TEST(ZKCLIENT, CLOSE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
}

TEST(ZKCLIENT, CLEAR) {
  using namespace cczk;
}

TEST(ZKCLIENT, IS_AVALIABLE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
  ASSERT_FALSE(tmp->is_avaiable());
}

TEST(ZKCLIENT, GET_CHILDREN_OF_PATH) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}

TEST(ZKCLIENT, SET_DATA_OF_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->create_node("/set_data_test", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  string data = "test";
  ret = tmp->set_data_of_node("/set_data_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->get_data_of_node("/set_data_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ASSERT_EQ(data, "test");
  ret = tmp->set_data_of_node("/set_data_test", null_str);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->get_data_of_node("/set_data_test", data);
  ASSERT_EQ(data, null_str);
  ret = tmp->set_data_of_node("/foobar", data);
  ASSERT_EQ(ret, ReturnCode::NoNode);
  ret = tmp->delete_node("/set_data_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
}

TEST(ZKCLIENT, GET_DATA_OF_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->create_node("/get_data_test", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  string data;
  ret = tmp->get_data_of_node("/foobar", data);
  ASSERT_EQ(ret, ReturnCode::NoNode);
}

TEST(ZKCLIENT, CREATE_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->create_node("/seq_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->create_node("/seq_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NodeExists);
  ret = tmp->create_node("/seq_test/foo/bar", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NoNode);
  
  ret = tmp->create_node("/seq_test/ephemeral_node", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->create_node("/seq_test/ephemeral_node/test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NoChildrenForEphemerals);
  
  ret = tmp->exist("/seq_test/ephemeral_node");
  ASSERT_EQ(ret, ReturnCode::Ok);
  tmp->close();
  tmp = zkclient::open(&config);
  ret = tmp->exist("/seq_test/ephemeral_node");
  ASSERT_EQ(ret, ReturnCode::NoNode);
  string data = "thisistest";
  ret = tmp->create_node("/seq_test/test", data, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  string data2;
  ret = tmp->get_data_of_node("/seq_test/test", data2);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ASSERT_EQ(data, data2);
  ret = tmp->delete_node("/seq_test/test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->delete_node("/seq_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  tmp->close();
}


TEST(ZKCLIENT, DELETE_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}

TEST(ZKCLIENT, EXIST) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
}

TEST(ZKCLIENT, ADD_LISTENER) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}


TEST(ZKCLIENT, DROP_LISTENER) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}


TEST(ZKCLIENT, DROP_LISTENER_WITH_PATH) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_NE(tmp, NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}