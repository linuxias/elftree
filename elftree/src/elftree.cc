#include <list>
#include <string>
#include <stdexcept>

#include "elftree.h"

TreeItem::TreeItem(const std::string fileName) :
  _fileName(fileName),
  _index(0),
  _depth(0),
  _folded(false)
{
  _elf = new ElfInfo(_fileName);
}

TreeItem::TreeItem(ElfInfo *elf) : _elf(elf),
  _index(0),
  _depth(0),
  _folded(false)
{
  _fileName = _elf->getFileName();
}

TreeItem::~TreeItem()
{
  for (TreeItem* child : _children)
    delete child;

  if (_elf != nullptr)
    delete _elf;
}

std::list<TreeItem*> TreeItem::getChildren() { return _children; }

ElfInfo *TreeItem::getElfInfo(void) { return _elf; }

void TreeItem::addChildItem(TreeItem *child)
{
  if (child == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _children.push_back(child);
}

TreeView::~TreeView() {
  if (_root == nullptr)
    return;

  delete _root;
}


