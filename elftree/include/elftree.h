#ifndef __ELFTREE_H__
#define __ELFTREE_H__

#include <list>
#include <string>

#include "elfinfo.h"

class TreeItem {
  private:
    bool _folded;
    std::string _fileName;
    ElfInfo *_elf;
    std::list<TreeItem*> _children;

  public:
    explicit TreeItem(std::string fileName);
    explicit TreeItem(ElfInfo* elf);
    ~TreeItem();
    std::string getFileName() { return _fileName; };
    std::list<TreeItem*> getChildren();
    void addChildItem(TreeItem *child);
    ElfInfo *getElfInfo(void);
    void setFolded(bool fold) { _folded = fold; };
    bool isFolded(void) { return _folded; };
    bool hasChildren(void) { return !_children.empty(); };
};

class TreeView {
  private:
    TreeItem* _root;
    TreeItem* _current;

  public:
    explicit TreeView(TreeItem *root) : _root(root), _current(nullptr) {};
    ~TreeView();
    TreeItem* getRootItem() { return _root; };
};

#endif /* __ELFTREE_H__ */
