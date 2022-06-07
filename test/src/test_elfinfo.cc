#include <exception>
#include <string>
#include <memory>

#include <ApprovalTests.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include "../../elftree/include/elfinfo.h"
#include "test_arch.h"

using namespace testing;
namespace bf=boost::filesystem;
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

TEST(ElfInfoTest, test_create_instance_with_non_elf_file) {
  EXPECT_THROW ({
    try {
      std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>("testdata/nonelf");
    } catch (const std::invalid_argument &e) {
      EXPECT_STREQ("This file is not ELF", e.what());
      throw e;
    }
  },
  std::invalid_argument);
}

TEST(ElfInfoTest, test_getFileName) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  EXPECT_EQ(info->getFileName(), fileName);
}

TEST(ElfInfoTest, test_getArchType) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  EXPECT_EQ(info->getArchType(), archType);
}

TEST(ElfInfoTest, test_getDependency) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  std::list<std::string> deps = info->getDependency();
  std::list<std::string> comp_deps = {
    "libncurses.so",
    "libtinfo.so",
    "libmenu.so",
    "libboost_filesystem.so",
    "libstdc++.so",
    "libgcc_s.so",
    "libc.so"
  };

  deps.sort();
  comp_deps.sort();

  std::list<std::string>::iterator it1 = deps.begin();
  std::list<std::string>::iterator it2 = comp_deps.begin();

  std::string delimeter = ".so";
  while (it1 != deps.end() && it2 != comp_deps.end()) {
    std::string dep_string = *it1;
    std::string comp_string = *it2;

    dep_string.erase(dep_string.find(delimeter) + delimeter.length(), dep_string.length());
    EXPECT_STREQ(dep_string.c_str(), comp_string.c_str());
    ++it1;
    ++it2;
  }
}

TEST(ElfInfoTest, test_getElfHeaderFormat) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  ApprovalTests::Approvals::verify(info->getElfHeaderFormat());
}

TEST(ElfInfoTest, test_getProgramHeaderFormat) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  ApprovalTests::Approvals::verify(info->getProgramHeaderFormat());
}

TEST(ElfInfoTest, test_getSectionHeaderFormat) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  ApprovalTests::Approvals::verify(info->getSectionHeaderFormat());
}

TEST(ElfInfoTest, test_getSectionDumpFormat) {
  std::vector<std::string> sections = {
    ".interp", ".dynsym", ".dynstr", ".text", ".rodata", ".dynamic", ".data", ".bss"
  };

  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  std::string dump_data = "";
  for (auto& section : sections)
    dump_data += info->getSectionDumpFormat(section);

  ApprovalTests::Approvals::verify(dump_data);
}

TEST(ElfInfoTest, test_getSectionList) {
  std::unique_ptr<ElfInfo> info = std::make_unique<ElfInfo>(filePath);
  std::string sections = "";
  for (auto& section : info->getSectionList())
    sections += (section + "\n");
  ApprovalTests::Approvals::verify(sections);
}
