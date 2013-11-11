#include <zookeeper_config.h>
#include <gtest/gtest.h>
#include <fstream>

TEST(ZOOKEEPER_CONFIG, CONSTRUCTION) {
  using namespace cczk;
  zookeeper_config config;
  zookeeper_config config2("cluster", 3000, "/root");
  zookeeper_config config3("cluster", 3000, "root");
  ASSERT_EQ(config.get_host(), "");
  ASSERT_EQ(config.get_root(), "");
  ASSERT_EQ(config.get_session_timeout(), 0);
  ASSERT_EQ(config2.get_host(), "cluster");
  ASSERT_EQ(config2.get_root(), "root");
  ASSERT_EQ(config2.get_session_timeout(), 3000);
  ASSERT_EQ(config3.get_root(), "root");
}

TEST(ZOOKEEPER_CONFIG, LOAD_FROM_FILE) {
  using namespace cczk;
  using namespace std;
  ofstream fout("zk.conf");
  fout << "cluster=localhost:2181" << endl;
  fout << "#root=must_be_ignore" << endl;
  fout << "root=/test" << endl;
  fout << "root=/only_first_set_value_is_vaild" << endl;
  fout << "session_timeout=3000" << endl;
  fout << "auth=foo:bar,test:test" << endl;
  fout.flush();
  fout.close();
  zookeeper_config config;
  config.load_from_file("zk.conf");
  ASSERT_EQ(config.get_host(), "localhost:2181");
  ASSERT_EQ(config.get_root(), "test");
  ASSERT_EQ(config.get_session_timeout(), 3000);
}