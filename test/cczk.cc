#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include <logging.h>

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  cczk::set_xcs_glog_file(argv[0]);
  
  return RUN_ALL_TESTS();

};

