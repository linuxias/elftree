#ifndef __ELFTREE_H__
#define __ELFTREE_H__

#include <list>
#include <string>

#include "elf_util.h"

class TreeItem {
  private:
    std::string _fileName;
    ElfInfo *_elf;
    TreeItem* _parent;
    TreeItem* _prev;
    TreeItem* _next;
    std::list<TreeItem*> _childs;

  public:
    TreeItem(std::string fileName);
    TreeItem* getParentItem();
    void setParentItem(TreeItem *item);
    TreeItem* getNextItem();
    void setNextItem(TreeItem *item);
    TreeItem* getPrevItem();
    void setPrevItem(TreeItem *item);
    std::list<TreeItem*> getChildsItem();
    void addChildItem(TreeItem *child);
    ElfInfo *getElfInfo(void);
};

#endif /* __ELFTREE_H__ */
