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

#include "elftree.h"
#include "tui.h"

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
      default:
        break;
        /*
        wmove(infoWindow, 1, 1);
        wclrtobot(infoWindow);
        box(infoWindow, 0, 0);
        mvwprintw(infoWindow, 1, 1, "%d", c);
        wrefresh(infoWindow);
        */
    }
    wrefresh(menuWindow);
  }
}
