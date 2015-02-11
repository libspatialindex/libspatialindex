#include "test.h"

#include "sidx_api_test.h"

int main(int argc, char** argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
