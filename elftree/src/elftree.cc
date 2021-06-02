#include <list>
#include <string>
#include <stdexcept>

#include "elftree.h"

TreeItem::TreeItem(const std::string fileName) : _fileName(fileName)
{
  _elf = new ElfInfo(_fileName);
  _depth = 0;
  _folded = false;
}

TreeItem::TreeItem(ElfInfo *elf) : _elf(elf)
{
  _fileName = _elf->getFileName();
  _depth = 0;
  _folded = false;
}

TreeItem* TreeItem::getNextItem() { return _next; }

TreeItem* TreeItem::getPrevItem() { return _prev; }

TreeItem* TreeItem::getParentItem() { return _parent; }

std::list<TreeItem*> TreeItem::getChildsItem() { return _childs; }

ElfInfo *TreeItem::getElfInfo(void) { return _elf; }

void TreeItem::setParentItem(TreeItem *item)
{
  if (item == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _parent = item;
}

void TreeItem::setNextItem(TreeItem *item)
{
  if (item == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _next = item;
}

void TreeItem::setPrevItem(TreeItem *item)
{
  if (item == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _prev = item;
}

void TreeItem::addChildItem(TreeItem *child)
{
  if (child == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _childs.push_back(child);
}
