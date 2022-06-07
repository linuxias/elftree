#include <exception>
#include <string>
#include <memory>
#include <list>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../elftree/include/elftree.h"
#include "test_arch.h"

using namespace testing;

const std::string fileName = "elftest";

TEST(TreeItemTest, test_create_instance)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  EXPECT_NE(item, nullptr);
}

TEST(TreeItemTest, test_create_instance_exception)
{
  EXPECT_THROW ({
    try {
      std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>("TEST");
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("This file is not exists", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}

TEST(TreeItemTest, test_getFileName)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  EXPECT_EQ(fileName, item->getFileName());
}

TEST(TreeItemTest, test_isFolded)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  EXPECT_EQ(true, item->isFolded());
  item->setFolded(false);
  EXPECT_EQ(false, item->isFolded());
  item->setFolded(true);
  EXPECT_EQ(true, item->isFolded());
}

TEST(TreeItemTest, test_hasChildren)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  TreeItem* child = new TreeItem(filePath);

  item->addChildItem(child);
  EXPECT_EQ(item->hasChildren(), true);
}

TEST(TreeItemTest, test_getChildren_empyty)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  std::list<TreeItem*> children = item->getChildren();

  EXPECT_EQ(children.empty(), true);
}

TEST(TreeItemTest, test_getChildren_check_size)
{
  std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);
  TreeItem* child = new TreeItem(filePath);

  item->addChildItem(child);
  std::list<TreeItem*> children = item->getChildren();
  EXPECT_EQ(children.size(), 1);
}

TEST(TreeItemTest, test_addChild_empyty)
{
  EXPECT_THROW ({
    try {
      std::unique_ptr<TreeItem> item = std::make_unique<TreeItem>(filePath);

      item->addChildItem(nullptr);
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("Child is nullptr", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}
