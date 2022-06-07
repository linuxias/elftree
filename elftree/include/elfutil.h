#ifndef __ELF_UTIL_H__
#define __ELF_UTIL_H__

#include <string>

#include "elfinfo.h"
#include "elftree.h"

class ElfUtil {
  public:
    static TreeView* makeTreeView(const std::string& rootpath);
    static ElfInfo* getElfInfoByName(const std::string& name);
};

#endif /* __ELF_UTIL_H__ */
