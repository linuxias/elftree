#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../elftree/include/window.h"
#include "../../elftree/include/tree_selector.h"

using namespace testing;

TEST(TreeSelector, test_create_instance)
{
  std::unique_ptr<TreeSelector> menuWindow = std::make_unique<TreeSelector>(Point{0, 0}, Point{3, 3});
  EXPECT_NE(menuWindow, nullptr);
}

TEST(TreeSelector, test_setCursorPosition_with_negative)
{
  EXPECT_THROW ({
    try {
      std::unique_ptr<TreeSelector> menuWindow = std::make_unique<TreeSelector>(Point{0, 0}, Point{3, 3});
      EXPECT_NE(menuWindow, nullptr);
      menuWindow->setCursorPosition(-1);
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("Wrong index : -1", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}

extern "C" int item_Count(const MENU *menu) {
  return 10;
}

TEST(TreeSelector, test_setCursorPosition_with_over_value)
{
  EXPECT_THROW ({
    try {
      std::unique_ptr<TreeSelector> menuWindow = std::make_unique<TreeSelector>(Point{0, 0}, Point{3, 3});
      EXPECT_NE(menuWindow, nullptr);
      menuWindow->setCursorPosition(10);
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("Wrong index : 10", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}
