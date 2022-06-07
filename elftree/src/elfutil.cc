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

static bool __check_file_is_exists(const std::string& fileName)
{
  if ( boost::filesystem::exists(fileName) == false )
    return false;
  return true;
}

static bool __has_prefix_substr(const std::string& src, const std::string& match)
{
  if (src.substr(0, match.size()) == match)
    return true;
  return false;
}

static void __get_path_in_soconf(const std::string& path, std::list<std::string>& pathList)
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
  static std::list<std::string> pathList;

  if (!pathList.empty())
    return pathList;

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

  pathList.push_back("/lib"s);
  pathList.push_back("/usr/lib"s);
  pathList.push_back("/lib64"s);
  pathList.push_back("/usr/lib64"s);

  pathList.unique();

  return pathList;
}

bool __is_valid_path(const std::string& filepath)
{
  if (__check_file_is_exists(filepath) == false)
    return false;

  if (filepath.find("ld-linux") != std::string::npos)
    return false;

  return true;
}

void __insert_info_to_map(ElfInfo* info)
{
  std::string name = info->getFileName();
  if (elfInfoMap.count(name) == 0)
    elfInfoMap[name] = info;
}

void __make_children_item(TreeItem*& parentItem, const ElfArchType rootType, int* idx)
{
  ElfInfo* parentInfo = parentItem->getElfInfo();
  std::list<std::string> children_string = parentInfo->getDependency();
  std::list<std::string> libpaths = __get_libpath_list();

  TreeItem* childItem = nullptr;
  ElfInfo* childInfo = nullptr;
  for (auto& child_string : children_string) {
    for (auto& dirpath : libpaths) {
      std::string filepath = dirpath + "/" + child_string;

      if (__is_valid_path(filepath) == false)
        continue;

      try {
        if (elfInfoMap.find(child_string) != elfInfoMap.end()) {
          childInfo = elfInfoMap[child_string];
        } else {
          childInfo = new ElfInfo(filepath);
          if (rootType != childInfo->getArchType()) {
            delete childInfo;
            childInfo = nullptr;
            continue;
          }
          __insert_info_to_map(childInfo);
        }

        childItem = new TreeItem(childInfo);
      } catch (const std::exception &e) {
        throw;
      }
      parentItem->addChildItem(childItem);
      break;
    }
  }

  if (parentItem->hasChildren() == false)
    parentItem->setFolded(false);
}

TreeView* ElfUtil::makeTreeView(const std::string& rootpath)
{
  int idx = 0;
  ElfInfo* rootInfo = nullptr;
  TreeItem* rootItem = nullptr;
  try {
    rootInfo = new ElfInfo(rootpath);
    rootItem = new TreeItem(rootInfo);
  } catch (const std::exception &e) {
    throw;
  }

  __insert_info_to_map(rootInfo);
  ElfArchType rootType = rootInfo->getArchType();
  std::queue<TreeItem *> treeQ;
  treeQ.push(rootItem);

  while (treeQ.empty() == false) {
    TreeItem* parentItem = treeQ.front();
    treeQ.pop();

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
  return treeView;
}

ElfInfo* ElfUtil::getElfInfoByName(const std::string& name)
{
  if (elfInfoMap.count(name) == 0)
    return nullptr;

  return elfInfoMap[name];
}
