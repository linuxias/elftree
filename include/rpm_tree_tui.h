#ifndef __RPM_TREE_TUI_H__
#define __RPM_TREE_TUI_H__

#include <ncurses.h>
#include <menu.h>
#include <form.h>

class RpmTreeTUI {
  public:
    void initTerminal(void);

  private:
    ITEM **itemList;
    ITEM *currentItem;
    MENU *menuList;
    WINDOW *menuWindow;
    WINDOW *infoWindow;
    int maxX, maxY;

  protected:
    void createMenu(int x, int y);
    void createInfoWindow(int x, int y);
};

#endif /* __RPM_TREE_TUI_H__ */
