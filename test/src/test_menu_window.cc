#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../elftree/include/window.h"
#include "../../elftree/include/menu_window.h"

using namespace testing;

TEST(MenuWindow, test_create_instance)
{
  std::unique_ptr<MenuWindow> menuWindow = std::make_unique<MenuWindow>(Point{0, 0}, Point{3, 3});
  EXPECT_NE(menuWindow, nullptr);
}
