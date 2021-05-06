#include <string>
#include <algorithm>
#include <vector>

#include <stdlib.h>

#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "rpm_tree_tui.h"

void RpmTreeTUI::createMenu(int x, int y)
{
  int size = _menus.size();

  itemList = (ITEM **)calloc(size + 1, sizeof(ITEM *));
  for (int i = 0; i < size; i++)
    itemList[i] = new_item((const char*)_menus[i].c_str(), "");

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

void RpmTreeTUI::createInfoWindow(int x, int y) {
	infoWindow = newwin(y, x - 1, 0, maxX - x);
	box(infoWindow, 0, 0);
	wrefresh(infoWindow);
}

void RpmTreeTUI::initTerminal(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, maxY, maxX);
}

void RpmTreeTUI::setMenuList(std::vector<std::string> menus)
{
  _menus = menus;
}

void RpmTreeTUI::run(void)
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
