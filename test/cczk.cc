#include <gtest/gtest.h>
#include <logging.h>

int main(int argc, char **argv) {
  using namespace cczk;
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  cczk::set_xcs_glog_file(argv[0]);
  
  return RUN_ALL_TESTS();
};

