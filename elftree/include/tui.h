#ifndef __TUI_H__
#define __TUI_H__

#include <string>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

class ElfTreeTUI {
  public:
    void initTerminal(void);
    void run(void);
    void setMenuList(std::vector<std::string> menus);

  private:
    std::vector<std::string> _menus;
    ITEM **itemList;
    ITEM *currentItem;
    MENU *menuList;
    WINDOW *menuWindow;
    WINDOW *infoWindow;
    int maxX, maxY;
    void createMenu(int x, int y);
    void createInfoWindow(int x, int y);
};

#endif /* __TUI_H__ */
