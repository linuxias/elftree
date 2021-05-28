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

static void __travelView(std::vector<std::string> &menus, TreeItem *item)
{
  std::string data = item->getFileName();
  data.insert(0, "- ");
  for (int i = 0; i < item->getDepth() * 2; i++)
    data.insert(0, " ");
  menus.push_back(data);

  std::list<TreeItem*> childs = item->getChildsItem();
  if (childs.empty() == false) {
    for (auto& child : childs)
      __travelView(menus, child);
  }
}

static std::vector<std::string> __convertViewToList(TreeView* view)
{
  std::vector<std::string> menus;
  TreeItem* rootItem = view->getRootItem();
  __travelView(menus, rootItem);

  return menus;
}

void ElfTreeTUI::createMenu(int x, int y)
{
  _menus = __convertViewToList(_menuTreeView);
  int size = _menus.size();

  itemList = (ITEM **)calloc(size + 1, sizeof(ITEM *));
  for (int i = 0; i < size; i++) {
    itemList[i] = new_item((const char*)_menus[i].c_str(), "");
  }

  menuList = new_menu((ITEM **)itemList);
  menuWindow = newwin(y, x, 0, 0);

  keypad(menuWindow, TRUE);

  set_menu_win(menuList, menuWindow);
  set_menu_sub(menuList, derwin(menuWindow, 0, 0, 1, 1));
  set_menu_format(menuList, size, 1);

  /* Set menu mark to the string " * " */
  set_menu_mark(menuList, " * ");

  box(menuWindow, 0, 0);
  refresh();

  post_menu(menuList);
  wrefresh(menuWindow);
}

void ElfTreeTUI::createInfoWindow(int x, int y) {
	infoWindow = newwin(y, x - 1, 0, maxX - x);
	box(infoWindow, 0, 0);
	wrefresh(infoWindow);
}

void ElfTreeTUI::initTerminal(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, maxY, maxX);
}

void ElfTreeTUI::setMenuList(TreeView* view)
{
  _menuTreeView = view;
}

std::string __getItemName(ITEM*& item)
{
  std::string nameStr = item_name(item);
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

void printStringToWindow(ITEM*& currentItem, WINDOW*& window)
{
  std::string curItemName = __getItemName(currentItem);
  ElfInfo* elfInfo = ElfUtil::getElfInfoByName(curItemName);
  wmove(window, 1, 1);
  wclrtobot(window);
  box(window, 0, 0);

  int i = 1;
  for (auto& str : splitStringByLine(elfInfo->getElfHeaderFormat()))
    mvwprintw(window, i++, 2, "%s", str.c_str());

  mvwprintw(window, i++, 2, "%s", "\n");

  for (auto& str : splitStringByLine(elfInfo->getProgramHeaderFormat()))
    mvwprintw(window, i++, 2, "%s", str.c_str());

  wrefresh(window);
}

void ElfTreeTUI::run(void)
{
  createMenu(maxX / 3, maxY);
  createInfoWindow((maxX / 3) * 2, LINES);

  int c;
  while ((c = getch()) != KEY_F(1)) {
    switch(c) {
      case KEY_DOWN:
        menu_driver(menuList, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(menuList, REQ_UP_ITEM);
        break;
      case KEY_RIGHT: // unfolded
      case KEY_LEFT:  // folded
      case 10: // Enter Keycode
        currentItem = current_item(menuList);
        printStringToWindow(currentItem, infoWindow);
        break;
    }
    wrefresh(menuWindow);
  }
}

