#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "rpm_tree_tui.h"

void RpmTreeTUI::createMenu(int x, int y)
{
  /*
   * TODO: Change example list to rpm list
   */
#define ITEM_COUNT 4
  char *tmpList[] = {
    "lib1",
    "lib2",
    "lib3",
    (char *)NULL
  };


  itemList = (ITEM **)calloc(ITEM_COUNT, sizeof(ITEM *));
  for (int i = 0; i < ITEM_COUNT; i++)
    itemList[i] = new_item(tmpList[i], "");

  menuList = new_menu((ITEM **)itemList);
  menuWindow = newwin(y, x, 0, 0);

  keypad(menuWindow, TRUE);

  set_menu_win(menuList, menuWindow);
  set_menu_sub(menuList, derwin(menuWindow, 6, 38, 3, 1));
  set_menu_format(menuList, ITEM_COUNT, 1);

  /* Set menu mark to the string " * " */
  set_menu_mark(menuList, "* ");

  box(menuWindow, 0, 0);
  mvwaddch(menuWindow, 2, 0, ACS_LTEE);
  mvwhline(menuWindow, 2, 1, ACS_HLINE, maxX / 3 - 2);
  mvwaddch(menuWindow, 2, x, ACS_RTEE);
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

  createMenu(maxX / 3, maxY);
  createInfoWindow((maxX / 3) * 2, LINES);
}
