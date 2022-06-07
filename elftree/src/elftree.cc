#include <list>
#include <string>
#include <stdexcept>

#include "elftree.h"

TreeItem::TreeItem(const std::string filePath) :
  _folded(true)
{
  try {
  _elf = new ElfInfo(filePath);
  } catch (const std::exception& e){
    throw;
  }
  _fileName = _elf->getFileName();
}

TreeItem::TreeItem(ElfInfo *elf) :
  _folded(true),
  _elf(elf)
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

std::list<TreeItem*> TreeItem::getChildren()
{
  return _children;
}

ElfInfo *TreeItem::getElfInfo(void)
{
  return _elf;
}

void TreeItem::addChildItem(TreeItem *child)
{
  if (child == nullptr)
    throw std::invalid_argument("Child is nullptr");
  _children.push_back(child);
}

TreeView::~TreeView()
{
  if (_root == nullptr)
    return;

  delete _root;
}
