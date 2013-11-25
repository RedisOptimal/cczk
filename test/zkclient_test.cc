#include <zkclient.h>
#include <fstream>
#include <zookeeper_config.h>
#include <watcher_factory.h>
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
  tmp->close();
  ASSERT_FALSE(tmp->is_avaiable());
  ASSERT_EQ(tmp, zkclient::open(NULL));
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  config.set_session_timeout(2000);
  tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
}

TEST(ZKCLIENT, CLOSE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
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
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  tmp->close();
  tmp->clear();
  ASSERT_FALSE(tmp->is_avaiable());
}

TEST(ZKCLIENT, GET_CHILDREN_OF_PATH) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->create_node("/get_child_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  for (int i = 0;i <= 9;++i) {
    std::stringstream ss;
    string buffer;
    ss << "/get_child_test/" << i;
    ss >> buffer;
    ret = tmp->create_node(buffer, null_str, CreateMode::Persistent);
    ASSERT_EQ(ret, ReturnCode::Ok);
  }
  std::vector<string> children;
  ret = tmp->get_children_of_path("/get_child_test", children);
  ASSERT_EQ(children.size(), 10);
  for (int i = 0;i <= 9;++i) {
    std::stringstream ss;
    string buffer;
    ss << "/get_child_test/" << i;
    ss >> buffer;
    ret = tmp->delete_node(buffer);
    ASSERT_EQ(ret, ReturnCode::Ok);
  }
  ret = tmp->delete_node("/get_child_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
}

TEST(ZKCLIENT, SET_DATA_OF_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
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
  ASSERT_TRUE(tmp != NULL);
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
  ASSERT_TRUE(tmp != NULL);
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
  tmp->clear();
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
  tmp->clear();
}


TEST(ZKCLIENT, DELETE_NODE) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  
}

TEST(ZKCLIENT, EXIST) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
}

class Stupid {
public:
  explicit Stupid(char *filename) {
    fout.open(filename,std::ios::out);
    _counter = 0;
  }
  
  void stupid_listener(const std::string &path, cczk::WatchEvent::type type) {
    _path = path;
    _watch_type = type;
    _counter++;
    fout << "Stupid Listener is running on [ path: " << path << "; Counter : " <<
    _counter << "; Type : " << cczk::WatchEvent::toString(type) << " ]\n";
    std::vector<std::string> ret;
    cczk::zkclient *tmp = cczk::zkclient::open(NULL);
    cczk::ReturnCode::type ret_code = tmp->get_children_of_path(path, ret);
    fout << "Counter of Children : " << ret.size() << " RETURN_CODE=" << cczk::ReturnCode::toString(ret_code) << std::endl;
    fout.flush();
  }
  
  ~Stupid() {
    fout.close();
  }
  std::ofstream fout;
  std::string _path;
  cczk::WatchEvent::type _watch_type;
  int _counter;
};

TEST(ZKCLIENT, ADD_LISTENER) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->exist("/listener_test") == ReturnCode::Ok) {
    tmp->delete_node("/listener_test");
  }
  ret = tmp->create_node("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("add_listener.out");
  watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<watcher> watch = watcher_factory::get_watcher(listener);
  ret = tmp->add_listener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->create_node("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->set_data_of_node("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->close();
}


TEST(ZKCLIENT, DROP_LISTENER) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->exist("/listener_test") == ReturnCode::Ok) {
    tmp->delete_node("/listener_test");
  }
  ret = tmp->create_node("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("drop_listener.out");
  watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<watcher> watch = watcher_factory::get_watcher(listener);
  ret = tmp->add_listener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->create_node("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->drop_listener(watch);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->set_data_of_node("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->close();  
  
}


TEST(ZKCLIENT, DROP_LISTENER_WITH_PATH) {
  using namespace cczk;
  zookeeper_config config("localhost:2181", 3000, "/test");
  zkclient *tmp = zkclient::open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->is_avaiable());
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->exist("/listener_test") == ReturnCode::Ok) {
    tmp->delete_node("/listener_test");
  }
  ret = tmp->create_node("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("drop_listener_with_path.out");
  watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<watcher> watch = watcher_factory::get_watcher(listener);
  ret = tmp->add_listener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->create_node("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->set_data_of_node("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->delete_node("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->close();  
  
}