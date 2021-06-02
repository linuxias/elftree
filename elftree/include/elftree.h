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
    void setFolded(bool fold) { _folded = fold; };
    bool isFolded(void) { return _folded; };
    bool hasChilds(void) { return !_childs.empty(); };
};

class TreeView {
  private:
    unsigned int _cntOfNodes;
    TreeItem* _root;
    TreeItem* _current;

  public:
    TreeView(TreeItem *root) : _root(root) {};
    TreeItem* getRootItem() { return _root; };
    unsigned int getCountOfNodes(void) { return _cntOfNodes; };
    void setCountOfNodes(unsigned int count) { _cntOfNodes = count; };
};

#endif /* __ELFTREE_H__ */
