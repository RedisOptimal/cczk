#include <zkclient.h>
#include <fstream>
#include <zookeeper_config.h>
#include <watcher_factory.h>
#include <gtest/gtest.h>


TEST(ZKCLIENT, OPEN) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
  config.set_host("no-Exist:2181");
  tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp == NULL);
  
  config.set_host("localhost:2181");
  config.set_root("");
  tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  tmp->Close();
  ASSERT_FALSE(tmp->IsAvailable());
  ASSERT_EQ(tmp, ZkClient::Open(NULL));
  tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  config.set_session_timeout(2000);
  tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  tmp->Close();
}

TEST(ZKCLIENT, CLOSE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  tmp->Close();
}

TEST(ZKCLIENT, CLEAR) {
  using namespace xcs::cczk;
}

TEST(ZKCLIENT, IS_AVALIABLE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  tmp->Close();
  tmp->Clear();
  ASSERT_FALSE(tmp->IsAvailable());
}

TEST(ZKCLIENT, GET_CHILDREN_OF_PATH) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->CreateNode("/get_child_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  for (int i = 0;i <= 9;++i) {
    std::stringstream ss;
    string buffer;
    ss << "/get_child_test/" << i;
    ss >> buffer;
    ret = tmp->CreateNode(buffer, null_str, CreateMode::Persistent);
    ASSERT_EQ(ret, ReturnCode::Ok);
  }
  std::vector<string> children;
  ret = tmp->GetChildrenOfPath("/get_child_test", children);
  ASSERT_EQ(children.size(), 10);
  for (int i = 0;i <= 9;++i) {
    std::stringstream ss;
    string buffer;
    ss << "/get_child_test/" << i;
    ss >> buffer;
    ret = tmp->DeleteNode(buffer);
    ASSERT_EQ(ret, ReturnCode::Ok);
  }
  ret = tmp->DeleteNode("/get_child_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
}

TEST(ZKCLIENT, SET_DATA_OF_NODE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->CreateNode("/set_data_test", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  string data = "test";
  ret = tmp->SetDataOfNode("/set_data_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->GetDataOfNode("/set_data_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ASSERT_EQ(data, "test");
  ret = tmp->SetDataOfNode("/set_data_test", null_str);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->GetDataOfNode("/set_data_test", data);
  ASSERT_EQ(data, null_str);
  ret = tmp->SetDataOfNode("/foobar", data);
  ASSERT_EQ(ret, ReturnCode::NoNode);
  ret = tmp->DeleteNode("/set_data_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
}

TEST(ZKCLIENT, GET_DATA_OF_NODE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->CreateNode("/get_data_test", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  string data;
  ret = tmp->GetDataOfNode("/foobar", data);
  ASSERT_EQ(ret, ReturnCode::NoNode);
}

TEST(ZKCLIENT, CREATE_NODE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  string null_str = "";
  ReturnCode::type ret;
  ret = tmp->CreateNode("/seq_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->CreateNode("/seq_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NodeExists);
  ret = tmp->CreateNode("/seq_test/foo/bar", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NoNode);
  
  ret = tmp->CreateNode("/seq_test/ephemeral_node", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->CreateNode("/seq_test/ephemeral_node/test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::NoChildrenForEphemerals);
  
  ret = tmp->Exist("/seq_test/ephemeral_node");
  ASSERT_EQ(ret, ReturnCode::Ok);
  tmp->Close();
  tmp->Clear();
  tmp = ZkClient::Open(&config);
  ret = tmp->Exist("/seq_test/ephemeral_node");
  ASSERT_EQ(ret, ReturnCode::NoNode);
  string data = "thisistest";
  ret = tmp->CreateNode("/seq_test/test", data, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  string data2;
  ret = tmp->GetDataOfNode("/seq_test/test", data2);
  ASSERT_EQ(ret, ReturnCode::Ok);
  ASSERT_EQ(data, data2);
  ret = tmp->DeleteNode("/seq_test/test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->DeleteNode("/seq_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  tmp->Close();
  tmp->Clear();
}


TEST(ZKCLIENT, DELETE_NODE) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
  
}

TEST(ZKCLIENT, EXIST) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
}

class Stupid {
public:
  explicit Stupid(char *filename) {
    fout.open(filename,std::ios::out);
    _counter = 0;
  }
  
  void stupid_listener(const std::string &path, xcs::cczk::WatchEvent::type type) {
    using namespace xcs::cczk;
    _path = path;
    _watch_type = type;
    _counter++;
    fout << "Stupid Listener is running on [ path: " << path << "; Counter : " <<
    _counter << "; Type : " << xcs::cczk::WatchEvent::toString(type) << " ]\n";
    ZkClient *tmp = ZkClient::Open(NULL);
    if (type == WatchEvent::ZnodeChildrenChanged) {
      std::vector<std::string> ret;
      ReturnCode::type ret_code = tmp->GetChildrenOfPath(path, ret);
      fout << "Counter of Children : " << ret.size() << " RETURN_CODE=" << ReturnCode::toString(ret_code) << std::endl;
    } else {
      std::string ret;
      ReturnCode::type ret_code = tmp->GetDataOfNode(path, ret);
      fout << "Data of Node : " << ret << "RETURN_CODE=" << ReturnCode::toString(ret_code) << std::endl;
    }
    fout.flush();
  }
  
  ~Stupid() {
    fout.close();
  }
  std::ofstream fout;
  std::string _path;
  xcs::cczk::WatchEvent::type _watch_type;
  int _counter;
};

TEST(ZKCLIENT, ADD_LISTENER) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->Exist("/listener_test") == ReturnCode::Ok) {
    tmp->DeleteNode("/listener_test");
  }
  ret = tmp->CreateNode("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("add_listener.out");
  Watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<Watcher> watch = WatcherFactory::get_watcher(listener, true, true);
  ret = tmp->AddListener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->CreateNode("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->SetDataOfNode("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->Close();
}


TEST(ZKCLIENT, DROP_LISTENER) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->Exist("/listener_test") == ReturnCode::Ok) {
    tmp->DeleteNode("/listener_test");
  }
  ret = tmp->CreateNode("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("drop_listener.out");
  Watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<Watcher> watch = WatcherFactory::get_watcher(listener, true, true);
  ret = tmp->AddListener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->CreateNode("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DropListener(watch);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->AddListener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Error);
  sleep(2);
  watch = WatcherFactory::get_watcher(listener, true, true);
  ret = tmp->AddListener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->SetDataOfNode("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->Close();  
  
}


TEST(ZKCLIENT, DROP_LISTENER_WITH_PATH) {
  using namespace xcs::cczk;
  ZookeeperConfig config("localhost:2181", 3000, "/test");
  ZkClient *tmp = ZkClient::Open(&config);
  ASSERT_TRUE(tmp != NULL);
  ASSERT_TRUE(tmp->IsAvailable());
  string null_str = "";
  ReturnCode::type ret;
  if (tmp->Exist("/listener_test") == ReturnCode::Ok) {
    tmp->DeleteNode("/listener_test");
  }
  ret = tmp->CreateNode("/listener_test", null_str, CreateMode::Persistent);
  ASSERT_EQ(ret, ReturnCode::Ok);
  
  Stupid stupid("drop_listener_with_path.out");
  Watcher::Listener listener = boost::bind(&Stupid::stupid_listener, &stupid, _1, _2);
  boost::shared_ptr<Watcher> watch = WatcherFactory::get_watcher(listener, true, true);
  ret = tmp->AddListener(watch, "/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  ret = tmp->CreateNode("/listener_test/child1", null_str, CreateMode::Ephemeral);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test/child1");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  string data = "thisistest";
  ret = tmp->SetDataOfNode("/listener_test", data);
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  ret = tmp->DeleteNode("/listener_test");
  ASSERT_EQ(ret, ReturnCode::Ok);
  sleep(2);
  tmp->Close();  
  
}