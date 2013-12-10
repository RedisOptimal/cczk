#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include <xcs/logging.h>

int main(int argc, char **argv) {
  using namespace xcs;
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  xcs::set_xcs_glog_file(argv[0]);
  
  return RUN_ALL_TESTS();

};

