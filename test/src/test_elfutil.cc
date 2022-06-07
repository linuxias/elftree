#include <exception>
#include <string>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/resource.h>

#include <ApprovalTests.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include "../../elftree/include/elfutil.h"
#include "../../elftree/include/elftree.h"

#include "test_arch.h"

TEST (ElfUtilTest, test_getElfInfoByName_without_makeTreeView) {
  ElfInfo* info = ElfUtil::getElfInfoByName("elftest");
  EXPECT_EQ(info, nullptr);
}

TEST (ElfUtilTest, test_getElfInfoByName_with_makeTreeView) {
  TreeView* view = ElfUtil::makeTreeView(filePath);
  ElfInfo* info = ElfUtil::getElfInfoByName("elftest");
  EXPECT_NE(info, nullptr);
}

TEST (ElfUtilTest, test_makeTreeView_exception_with_wrong_file) {
  EXPECT_THROW ({
    try {
      TreeView* view = ElfUtil::makeTreeView("Test");
      delete view;
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("This file is not exists", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}

static int __get_file_max_count()
{
  struct rlimit rlp;
  getrlimit(RLIMIT_NOFILE, &rlp);
  return rlp.rlim_max;
}

TEST (ElfUtilTest, test_makeTreeView_exception_with_max_file) {
  int file_max_cnt = __get_file_max_count();
  std::vector<int> fd_list;

  for (int i = 0; i < file_max_cnt; i++)
    fd_list.push_back(open(filePath.c_str(), O_RDONLY));

  EXPECT_THROW ({
    try {
      TreeView* view = ElfUtil::makeTreeView(filePath);
      delete view;
    } catch (const std::ios_base::failure &e) {
      EXPECT_STREQ("Failed to create elfinfo : Too many open files: iostream error", e.what());
      for (int fd : fd_list)
        close(fd);
      throw e;
    }
  },
  std::ios_base::failure);
}
