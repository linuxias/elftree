#ifndef __TUI_H__
#define __TUI_H__

#include <string>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "elftree.h"

class ElfTreeTUI {
  public:
    void initTerminal(void);
    void clearTerminal(void);
    void run(void);
    void setMenuList(TreeView* view);

  private:
    std::vector<std::string> _menus;
    TreeView* _menuTreeView;
    ITEM **itemList;
    ITEM *currentItem;
    MENU *menuList;
    WINDOW *menuWindow;
    WINDOW *infoWindow;
    int maxX, maxY;
    void createMenu(int x, int y);
    void clearMenu(void);
    void createInfoWindow(int x, int y);
    void clearInfoWindow(void);
    void convertViewToItems(TreeView* view);
    void travelTree(TreeItem*& item, int* index);
    void toggleMenu(bool fold);
};

#endif /* __TUI_H__ */
