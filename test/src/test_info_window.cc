#include <ApprovalTests.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../elftree/include/window.h"
#include "../../elftree/include/info_window.h"

using namespace testing;

TEST(InfoWindow, test_create_instance) {
  std::unique_ptr<InfoWindow> info = std::make_unique<InfoWindow>(Point{0, 0}, Point{3, 3});
  EXPECT_NE(info, nullptr);
}
