//
// Created by tao on 19-1-17.
//
#include "common_includes.h"
#include "truncatable_file.h"
TEST(test_test, 1) {
  TruncatableFile tf{};
  ASSERT_EQ(tf.init("hello.txt"), 0);
  ASSERT_EQ(tf.truncate_file(), 0);

  char buff[1024];
  for (int i = 0; i < 100; ++i) {
    snprintf(buff, sizeof(buff), "now at %d", i);
    ASSERT_EQ(tf.truncate_write(buff, strlen(buff)), 0);
  }
  auto all_data = tf.read_all();
  ASSERT_EQ(all_data, "now at 99");
}

int main(int argc, char **argv) {
  int iRet = 0;
  iRet = beforeRun();
  if (iRet) {
    std::cerr << "init fail with " << iRet << std::endl;
  }
  testing::InitGoogleTest(&argc, argv);
  iRet = RUN_ALL_TESTS();
  return iRet;
}