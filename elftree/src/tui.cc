#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <list>

#include <stdlib.h>

#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include <boost/algorithm/string.hpp>

#include "elftree.h"
#include "elfutil.h"
#include "tui.h"

using namespace std::string_literals;
using namespace boost::algorithm;

void ElfTreeTUI::travelTree(TreeItem*& item, int* index)
{
  std::string data = item->getFileName();
  if (item->isFolded())
    data.insert(0, "+ ");
  else
    data.insert(0, "- ");
  for (int i = 0; i < item->getDepth() * 2; i++)
    data.insert(0, " ");

  itemList[*index] = new_item((const char*)(strdup(data.c_str())), "");
  set_item_userptr(itemList[*index], item);

  if (item->isFolded() == true)
    return;

  std::list<TreeItem*> children = item->getChildren();
  if (children.empty() == false) {
    for (TreeItem* child : children) {
      *index = *index + 1;
     travelTree(child, index);
    }
  }
}

void ElfTreeTUI::convertViewToItems(TreeView* view)
{
  TreeItem* rootItem = view->getRootItem();
  int index = 0;

  travelTree(rootItem,  &index);
}


void ElfTreeTUI::createMenu(int x, int y)
{
  int size = _menuTreeView->getCountOfNodes();
  itemList = (ITEM **)calloc(size + 1, sizeof(ITEM *));
  convertViewToItems(_menuTreeView);
  menuList = new_menu((ITEM **)itemList);
  menuWindow = newwin(y, x, 0, 0);

  keypad(menuWindow, TRUE);

  set_menu_win(menuList, menuWindow);
  set_menu_sub(menuList, derwin(menuWindow, 0, 0, 1, 1));
  set_menu_format(menuList, size, 1);

  /* Set menu mark to the string " * " */
  set_menu_mark(menuList, " * ");

  refresh();
  post_menu(menuList);
  box(menuWindow, 0, 0);
  wrefresh(menuWindow);
}

void ElfTreeTUI::clearMenu(void)
{
  unpost_menu(menuList);
  free_menu(menuList);
  delwin(menuWindow);
}

void ElfTreeTUI::createInfoWindow(int x, int y) {
	infoWindow = newwin(y, x - 1, 0, maxX - x);
  scrollok(infoWindow, TRUE);

	box(infoWindow, 0, 0);
	wrefresh(infoWindow);
}

void ElfTreeTUI::clearInfoWindow(void)
{
  delwin(infoWindow);
}

void ElfTreeTUI::initTerminal(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, maxY, maxX);
}

void ElfTreeTUI::clearTerminal(void)
{
  clearMenu();
  clearInfoWindow();
  endwin();
}

void ElfTreeTUI::setMenuList(TreeView* view)
{
  _menuTreeView = view;
}

std::string __getItemName(ITEM*& item)
{
  std::string nameStr = item_name(item);
  TreeItem* treeItem = (TreeItem*)item_userptr(item);
  if (treeItem->isFolded())
    nameStr = nameStr.substr(nameStr.find("+") + 1, nameStr.length());
  else
    nameStr = nameStr.substr(nameStr.find("-") + 1, nameStr.length());

  trim(nameStr);

  return nameStr;
}

std::vector<std::string> splitStringByLine(std::string s) {
  std::vector<std::string> v;
	int start = 0;
	int d = s.find("\n");
	while (d != -1){
		v.push_back(s.substr(start, d - start));
		start = d + 1;
		d = s.find("\n", start);
	}
	v.push_back(s.substr(start, d - start));

	return v;
}

void printStringToWindow(ITEM*& currentItem, WINDOW*& window, int i)
{
  std::string curItemName = __getItemName(currentItem);
  ElfInfo* elfInfo = ElfUtil::getElfInfoByName(curItemName);
  wmove(window, 1, 1);
  wclrtobot(window);
  box(window, 0, 0);

  for (auto& str : splitStringByLine(elfInfo->getElfHeaderFormat()))
    mvwprintw(window, i++, 2, "%s", str.c_str());

  mvwprintw(window, i++, 2, "%s", "\n");

  for (auto& str : splitStringByLine(elfInfo->getProgramHeaderFormat()))
    mvwprintw(window, i++, 2, "%s", str.c_str());

  for (auto& str : splitStringByLine(elfInfo->getSectionHeaderFormat()))
    mvwprintw(window, i++, 2, "%s", str.c_str());

  box(window, 0, 0);
  wrefresh(window);
}

void ElfTreeTUI::toggleMenu(bool fold)
{
  int pos;
  TreeItem* currentTreeItem;

  currentItem = current_item(menuList);
  currentTreeItem = (TreeItem*)item_userptr(currentItem);
  if (fold == true && (currentTreeItem->isFolded() == true || currentTreeItem->hasChildren() == false))
      return;
  else if (fold == false && currentTreeItem->isFolded() == false)
      return;

  pos = item_index(currentItem);
  currentTreeItem->setFolded(fold);
  clearMenu();
  createMenu(maxX / 3, maxY);
  set_current_item(menuList, itemList[pos]);
}

void ElfTreeTUI::run(void)
{
  currentItem = nullptr;
  createMenu(maxX / 3, maxY);
  createInfoWindow((maxX / 3) * 2, LINES);

  int c;
  int i  = 1;
  while ((c = getch()) != KEY_F(1)) {
    switch(c) {
      case KEY_NPAGE:
        if (currentItem == nullptr)
          break;
        wborder(infoWindow, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        printStringToWindow(currentItem, infoWindow, --i);
        break;
      case KEY_PPAGE:
        if (currentItem == nullptr)
          break;
        wborder(infoWindow, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        if (i >= 1) i = 1;
        printStringToWindow(currentItem, infoWindow, ++i);
        break;
      case KEY_DOWN:
        menu_driver(menuList, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(menuList, REQ_UP_ITEM);
        break;
      case KEY_RIGHT: // unfolded
        toggleMenu(false);
        break;
      case KEY_LEFT:  // folded
        toggleMenu(true);
        break;
      case 10: // Enter Keycode
        currentItem = current_item(menuList);
        i = 1;
        printStringToWindow(currentItem, infoWindow, i);
        break;
    }
    box(infoWindow, 0, 0);
    wrefresh(infoWindow);
    box(menuWindow, 0, 0);
    wrefresh(menuWindow);
  }
}
