#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "tui.h"
#include "elf_util.h"

using namespace std::string_literals;
using namespace boost::algorithm;

static void __print_usage()
{
  std::cout << "Usage : rpmtree {library name or file name}\n" << std::endl;
}

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

  pathList.sort();
  pathList.unique();

  return pathList;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    __print_usage();
    return -1;
  }

  std::string fileName = std::string(argv[1]);
  if (__check_file_is_exists(fileName) == false) {
    std::cout << "Please check file is exists" << std::endl;
    return -1;
  }

  ElfInfo info(fileName);
  std::vector<std::string> deps = info.getDependency();

  ElfTreeTUI tui;
  tui.initTerminal();
  tui.setMenuList(deps);
  tui.run();

  return 0;
}
