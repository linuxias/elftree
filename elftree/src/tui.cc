#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <list>

#include <stdlib.h>

#include <unistd.h>

#include <boost/algorithm/string.hpp>

#include "elftree.h"
#include "elfutil.h"
#include "tui.h"

#include "section_selector.h"
#include "addrdump_text_editor.h"

using namespace std::string_literals;
using namespace boost::algorithm;

ElfTreeTUI::ElfTreeTUI()
{
  _infoWindow = nullptr;
  _treeSelector = nullptr;
  _menuTreeView = nullptr;
}

void ElfTreeTUI::travelTree(TreeItem*& item, int depth)
{
  std::string data = item->getFileName();
  if (item->isFolded())
    data.insert(0, "+ ");
  else
    data.insert(0, "- ");
  for (int i = 0; i < depth * 2; i++)
    data.insert(0, " ");

  _menus.push_back(new MenuItem{data, (void*)item});

  if (item->isFolded() == true)
    return;

  std::list<TreeItem*> children = item->getChildren();
  if (children.empty() == false) {
    for (TreeItem* child : children)
      travelTree(child, depth + 1);
  }
}

void ElfTreeTUI::convertViewToItems(TreeView* view)
{
  TreeItem* rootItem = view->getRootItem();

  _menus.clear();
  std::vector<MenuItem*>().swap(_menus);

  travelTree(rootItem, 0);
}

void ElfTreeTUI::toggleMenu(bool fold)
{
  int pos;
  MenuItem* currentMenuItem = _treeSelector->getCurrentItem();
  TreeItem* currentTreeItem = static_cast<TreeItem*>(currentMenuItem->userptr);

  if (fold == true && (currentTreeItem->isFolded() == true || currentTreeItem->hasChildren() == false))
      return;
  else if (fold == false && currentTreeItem->isFolded() == false)
      return;

  currentTreeItem->setFolded(fold);
  convertViewToItems(_menuTreeView);
  pos = _treeSelector->getCursorPosition();
  _treeSelector->unpost();
  _treeSelector->setMenuItems(_menus);
  _treeSelector->post();
  _treeSelector->setCursorPosition(pos);
}

void ElfTreeTUI::initTerminal(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  refresh();
}

void ElfTreeTUI::clearTerminal(void)
{
  delete _infoWindow;
  delete _treeSelector;
  endwin();
}

void ElfTreeTUI::setMenuList(TreeView* view)
{
  _menuTreeView = view;
}

void ElfTreeTUI::refreshTerminal(void)
{
  _infoWindow->refresh();
  _treeSelector->reset();
}

std::string __getItemRawName(MenuItem* item)
{
  std::string name = item->name;
  TreeItem* treeItem = static_cast<TreeItem*>(item->userptr);
  if (treeItem->isFolded())
    name = name.substr(name.find("+") + 1, name.length());
  else
    name = name.substr(name.find("-") + 1, name.length());
  trim(name);
  return name;
}

void ElfTreeTUI::printInformation(int start_line)
{
  _infoWindow->hideBox();
  MenuItem *menuItem = _treeSelector->getSelectedItem();
  std::string curItemName = __getItemRawName(menuItem);
  ElfInfo* tmpInfo = ElfUtil::getElfInfoByName(curItemName);

  std::string output = tmpInfo->getElfHeaderFormat();
  output += "\n";
  output += tmpInfo->getProgramHeaderFormat();
  output += "\n";
  output += tmpInfo->getSectionHeaderFormat();

  _infoWindow->printText(output, start_line);
  _infoWindow->showBox();
}

void ElfTreeTUI::printSection(std::string section)
{
  MenuItem* curItem = _treeSelector->getSelectedItem();
  std::string curItemName = __getItemRawName(curItem);
  ElfInfo* tmpInfo = ElfUtil::getElfInfoByName(curItemName);

  std::string output = tmpInfo->getSectionDumpFormat(section);
  _infoWindow->printText(output, 1);
  _infoWindow->showBox();
}

void ElfTreeTUI::printAddressDump(const std::string& address)
{
  MenuItem* curItem = _treeSelector->getSelectedItem();
  std::string curItemName = __getItemRawName(curItem);
  ElfInfo* tmpInfo = ElfUtil::getElfInfoByName(curItemName);

  unsigned int addrSize = std::stoul(address, nullptr, 16);
  unsigned int size = tmpInfo->getELFSize() - addrSize;

  std::string output = tmpInfo->getMemDumpFormat(address, size);
  _infoWindow->printText(output, 1);
  _infoWindow->showBox();
}

std::string ElfTreeTUI::selectSection(void)
{
  SectionSelector* selector =
    new SectionSelector(
      {10, 10}, {40, 30});

  MenuItem* item = _treeSelector->getSelectedItem();
  if (item == nullptr)
    return "";
  std::string curItemName = __getItemRawName(item);
  ElfInfo* tmpInfo = ElfUtil::getElfInfoByName(curItemName);

  std::vector<MenuItem*> selected_menus;

  std::list<std::string> sections = tmpInfo->getSectionList();
  for (auto& section : sections)
    selected_menus.push_back(new MenuItem{section, nullptr});

  selector->setMenuItems(selected_menus);
  selector->post();
  MenuItem* return_item = selector->selectItem();
  if (return_item == nullptr)
    return "";

  return return_item->name;
}

std::string ElfTreeTUI::inputAddress(void)
{
  AddrDumpTextEditor* editor = new AddrDumpTextEditor(
      {_maxPoint.x/2 - 25, _maxPoint.y/2 - 2},
      {_maxPoint.x/2 + 25, _maxPoint.y/2 + 1});
  editor->show();
  std::string address = editor->editText();

  return address;
}

void ElfTreeTUI::run(void)
{
  _maxPoint = Window::getMaxXY();
  int menuX = _maxPoint.x / 3;
  int menuY = _maxPoint.y;
  int infoX = menuX * 2;

  convertViewToItems(_menuTreeView);
  _infoWindow = new InfoWindow(
      {menuX, 0},
      {infoX, _maxPoint.y});
  _treeSelector = new TreeSelector({0, 0}, {menuX, menuY});
  _treeSelector->setMenuItems(_menus);
  _treeSelector->post();
  _treeSelector->setCursorPosition(0);

  int start_line = 1;
  int c;
  std::string select_section;
  while ((c = getch()) != KEY_F(1)) {
    switch(c) {
      case KEY_RESIZE:
        refreshTerminal();
        break;
      case KEY_NPAGE:
        _infoWindow->upScroll();
        break;
      case KEY_PPAGE:
        _infoWindow->downScroll();
        break;
      case KEY_DOWN:
        _treeSelector->downCursor();
        break;
      case KEY_UP:
        _treeSelector->upCursor();
        break;
      case KEY_RIGHT: // unfolded
        toggleMenu(false);
        break;
      case KEY_LEFT:  // folded
        toggleMenu(true);
        break;
      case 10: // Enter Keycode
        _treeSelector->selectItem();
        start_line = 1;
        printInformation(start_line);
        break;
      case 115: //s
        _treeSelector->selectItem();
        select_section = selectSection();
        if (select_section.empty())
          break;
        printSection(select_section);
        break;
      case 100: //d
        _treeSelector->selectItem();
        std::string address = inputAddress();
        if (!address.empty())
          printAddressDump(address);
        break;
    }

    _infoWindow->showBox();
    _treeSelector->showBox();
  }
}
