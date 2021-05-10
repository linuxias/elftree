#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include "rpm_tree_tui.h"
#include "elf_util.h"

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

  RpmTreeTUI tui;
  tui.initTerminal();
  tui.setMenuList(deps);

  tui.run();

  return 0;
}
