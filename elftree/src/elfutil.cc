#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <queue>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "elfinfo.h"
#include "elfutil.h"

using namespace std::string_literals;
using namespace boost::algorithm;

static std::map<std::string, ElfInfo*> elfInfoMap;

static bool __check_file_is_exists(std::string fileName)
{
  if ( boost::filesystem::exists(fileName) == false )
    return false;
  return true;
}

static bool __has_prefix_substr(std::string src, std::string match)
{
  if (src.substr(0, match.size()) == match)
    return true;
  return false;
}

static void __get_path_in_soconf(const std::string path, std::list<std::string>& pathList)
{
  if (boost::filesystem::is_directory(path) == false)
    return;

  std::string buffer;
  for (const auto& f : boost::filesystem::directory_iterator(path)) {
    std::ifstream conf(f.path().string());

    while (conf.peek() != EOF) {
      getline(conf, buffer);
      trim(buffer);
      if (__has_prefix_substr(buffer, "#"))
        continue;

      pathList.push_back(buffer);
    }
  }
}

static std::list<std::string> __get_libpath_list(void)
{
  std::list<std::string> pathList = {
    "/lib"s,
    "/usr/lib"s,
    "/lib64"s,
    "/usr/lib64"s
  };

  char *ld_path = std::getenv("LD_LIBRARY_PATH");
  if (ld_path != nullptr)
    pathList.push_back(std::string(ld_path));

  std::ifstream confFile("/etc/ld.so.conf");
  if (confFile.is_open() != false) {

    std::string buffer;
    std::string prefix("include ");
    while (confFile.peek() != EOF) {
      getline(confFile, buffer);
      trim(buffer);
      if (__has_prefix_substr(buffer, prefix)) {
        std::string value = buffer.substr(prefix.size());
        size_t pos = value.find_last_of("/");
        __get_path_in_soconf(value.substr(0, pos), pathList);
      } else if (__has_prefix_substr(buffer, "#")) {
        continue;
      } else {
        if (boost::filesystem::is_directory(buffer))
          pathList.push_back(buffer);
      }
    }
  }

  pathList.unique();

  return pathList;
}

bool __is_valid_path(std::string filepath)
{
  if (__check_file_is_exists(filepath) == false)
    return false;

  if (filepath.find("ld-linux") != std::string::npos)
    return false;

  return true;
}

void __insert_info_in_map(ElfInfo* info)
{
  std::string name = info->getFileName();
  if (elfInfoMap.count(name) == 0)
    elfInfoMap[name] = info;
}

void __make_children_item(TreeItem*& parentItem, const ElfArchType rootType, int* idx)
{
  ElfInfo* parentInfo = parentItem->getElfInfo();
  std::list<std::string> childs_string = parentInfo->getDependency();
  std::list<std::string> libpaths = __get_libpath_list();

  for (auto& child_string : childs_string) {
    for (auto& dirpath : libpaths) {
      std::string filepath = dirpath + "/" + child_string;
      if (__is_valid_path(filepath) == false)
        continue;

      TreeItem* childItem = nullptr;
      if (elfInfoMap.find(child_string) != elfInfoMap.end()) {
        ElfInfo* childInfo = elfInfoMap[child_string];
        childItem = new TreeItem(childInfo);
      } else {
        ElfInfo* childInfo = new ElfInfo(filepath);
        if (rootType != childInfo->getArchType()) {
          delete childInfo;
          continue;
        }

        try {
          childItem = new TreeItem(childInfo);
        } catch (const std::exception &e) {
          throw;
        }

        __insert_info_in_map(childInfo);
      }

      childItem->setDepth(parentItem->getDepth() + 1);
      childItem->setIndex(++*idx);
      parentItem->addChildItem(childItem);
      break;
    }
  }
}

TreeView* ElfUtil::makeTreeView(std::string rootpath)
{
  int idx = 0;
  TreeItem* rootItem = nullptr;
  try {
    rootItem = new TreeItem(rootpath);
  } catch (const std::exception &e) {
    throw;
  }

  std::queue<TreeItem *> treeQ;
  treeQ.push(rootItem);

  ElfInfo* rootInfo = rootItem->getElfInfo();
  __insert_info_in_map(rootInfo);

  ElfArchType rootType = rootInfo->getArchType();

  while (treeQ.empty() == false) {
    TreeItem* parentItem = treeQ.front();
    treeQ.pop();

    __insert_info_in_map(parentItem->getElfInfo());

    try {
      __make_children_item(parentItem, rootType, &idx);
    } catch (const std::exception &e) {
      delete rootItem;
      throw;
    }
    std::list<TreeItem*> children = parentItem->getChildren();
    for (TreeItem* child : children)
      treeQ.push(child);
  }

  TreeView* treeView = new TreeView(rootItem);
  treeView->setCountOfNodes(idx);
  return treeView;
}

ElfInfo* ElfUtil::getElfInfoByName(std::string name)
{
  if (elfInfoMap.count(name) == 0)
    return nullptr;

  return elfInfoMap[name];
}
