#ifndef __ELFTREE_H__
#define __ELFTREE_H__

#include <list>
#include <string>

#include "elfinfo.h"

class TreeItem {
  private:
    int _index;
    int _depth;
    bool _folded;
    std::string _fileName;
    ElfInfo *_elf;
    std::list<TreeItem*> _children;

  public:
    TreeItem(std::string fileName);
    TreeItem(ElfInfo* elf);
    ~TreeItem();
    std::string getFileName() { return _fileName; };
    std::list<TreeItem*> getChildren();
    void addChildItem(TreeItem *child);
    ElfInfo *getElfInfo(void);
    void setDepth(int depth) { _depth = depth; };
    int getDepth(void) { return _depth; }
    void setIndex(int index) { _index = index; };
    int getIndex(void) { return _index; }
    void setFolded(bool fold) { _folded = fold; };
    bool isFolded(void) { return _folded; };
    bool hasChildren(void) { return !_children.empty(); };
};

class TreeView {
  private:
    unsigned int _cntOfNodes;
    TreeItem* _root;
    TreeItem* _current;

  public:
    TreeView(TreeItem *root) : _root(root), _cntOfNodes(0), _current(nullptr) {};
    ~TreeView();
    TreeItem* getRootItem() { return _root; };
    unsigned int getCountOfNodes(void) { return _cntOfNodes; };
    void setCountOfNodes(unsigned int count) { _cntOfNodes = count; };
};

#endif /* __ELFTREE_H__ */
