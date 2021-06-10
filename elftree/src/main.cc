#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "tui.h"
#include "elftree.h"
#include "elfinfo.h"
#include "elfutil.h"

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

  TreeView* view = nullptr;
  try {
    view = ElfUtil::makeTreeView(fileName);
  } catch (const std::exception &e) {
    std::cerr << "Failed to make TreeView : " << e.what() << std::endl;
    exit(-1);
  }

  ElfTreeTUI tui;
  tui.initTerminal();
  tui.setMenuList(view);
  tui.run();
  tui.clearTerminal();

  return 0;
}
