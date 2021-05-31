#include <exception>
#include <string>
#include <memory>

#include <ApprovalTests.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include "../../elftree/include/elfinfo.h"

using namespace testing;
namespace bf=boost::filesystem;

const std::string filePath = "testdata/elftest";
const std::string fileName = "elftest";

TEST(ElfInfoTest, test_create_instance) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  EXPECT_EQ(fileName, info->getFileName());
}

TEST(ElfInfoTest, test_create_instance_with_wrong_file) {
  EXPECT_THROW ({
    try {
      std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>("TEST");
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("This file is not exists", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}

TEST(ElfInfoTest, test_getAbsolutePath) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  std::string fileDir = bf::current_path().string();
  fileDir = fileDir + "/testdata";

  EXPECT_EQ(info->getAbsolutePath(), fileDir);
}

TEST(ElfInfoTest, test_getElfHeaderFormat) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  ApprovalTests::Approvals::verify(info->getElfHeaderFormat());
}
