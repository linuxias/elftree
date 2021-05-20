#ifndef __ELFTREE_H__
#define __ELFTREE_H__

#include <list>
#include <string>

#include "elfinfo.h"

class TreeItem {
  private:
    int _index;
    int _depth;
    std::string _fileName;
    ElfInfo *_elf;
    TreeItem* _parent;
    TreeItem* _prev;
    TreeItem* _next;
    std::list<TreeItem*> _childs;

  public:
    TreeItem(std::string fileName);
    TreeItem(ElfInfo* elf);
    std::string getFileName() { return _fileName; };
    TreeItem* getParentItem();
    void setParentItem(TreeItem *item);
    TreeItem* getNextItem();
    void setNextItem(TreeItem *item);
    TreeItem* getPrevItem();
    void setPrevItem(TreeItem *item);
    std::list<TreeItem*> getChildsItem();
    void addChildItem(TreeItem *child);
    ElfInfo *getElfInfo(void);
    void setDepth(int depth) { _depth = depth; };
    int getDepth(void) { return _depth; }
    void setIndex(int index) { _index = index; };
    int getIndex(void) { return _index; }
};

class TreeView {
  private:
    TreeItem* _root;
    TreeItem* _current;

  public:
    TreeView(TreeItem *root) : _root(root) {};
    TreeItem* getRootItem() { return _root; };
};

#endif /* __ELFTREE_H__ */
