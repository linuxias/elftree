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

static std::map<std::string, TreeItem*> elfInfoMap;

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

TreeView* ElfUtil::makeTreeView(std::string rootpath)
{
  int idx = 0;
  TreeItem* rootItem = new TreeItem(rootpath);
  std::queue<TreeItem *> treeQ;
  treeQ.push(rootItem);

  std::list<std::string> libpaths = __get_libpath_list();
  ElfInfo* rootInfo = rootItem->getElfInfo();
  ElfArchType rootType = rootInfo->getArchType();

  while (treeQ.empty() == false) {
    TreeItem* parentItem = treeQ.front();
    treeQ.pop();

    std::string parentName = parentItem->getFileName();
    if (elfInfoMap.count(parentName) == 0)
      elfInfoMap[parentName] = parentItem;

    ElfInfo* parentInfo = parentItem->getElfInfo();
    std::list<std::string> childs_string = parentInfo->getDependency();

    for (auto& child_string : childs_string) {
      TreeItem* prevItem = nullptr;
      for (auto& dirpath : libpaths) {
        std::string filepath = dirpath + "/" + child_string;
        if (__check_file_is_exists(filepath) == false)
          continue;

        ElfInfo *tmpInfo = new ElfInfo(filepath);
        std::string tmpFileName = tmpInfo->getFileName();
        if (rootType != tmpInfo->getArchType() || __has_prefix_substr(tmpFileName, "ld-linux-")) {
          delete tmpInfo;
          continue;
        }

        TreeItem* childItem = new TreeItem(tmpInfo);
        childItem->setDepth(parentItem->getDepth() + 1);
        childItem->setIndex(++idx);
        parentItem->addChildItem(childItem);
        childItem->setParentItem(parentItem);
        treeQ.push(childItem);

        if (prevItem != nullptr) {
          childItem->setPrevItem(prevItem);
          prevItem->setNextItem(childItem);
        }
        prevItem = childItem;
      }
    }
  }

  TreeView* treeView = new TreeView(rootItem);
  treeView->setCountOfNodes(idx);
  return treeView;
}

ElfInfo* ElfUtil::getElfInfoByName(std::string name)
{
  if (elfInfoMap.count(name) == 0)
    return nullptr;

  return elfInfoMap[name]->getElfInfo();
}
