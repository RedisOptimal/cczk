#include <iostream>
#include <boost/noncopyable.hpp>
#include <zookeeper_const.h>
#include <zookeeper_config.h>
#include <zkclient.h>
#include <watcher_factory.h>

int main(int argc, char **argv) {
  cczk::zookeeper_config config("10.3.20.161:21810",3000,"/test");
  cczk::zkclient *tmp = cczk::zkclient::Open(&config);
  std::cout << "Hello, world!" << std::endl;
  return 0;
};

