#ifndef __MENU_WINDOW_H__
#define __MENU_WINDOW_H__

#include <string>
#include <vector>

#include <ncurses.h>
#include <menu.h>
#include <form.h>

#include "window.h"

struct MenuItem {
  std::string name;
  void* userptr;
};

class MenuWindow : public Window {
  public:
    MenuWindow() = delete;
    MenuWindow(Point start = {0, 0}, Point end = {0, 0});
    virtual ~MenuWindow();
    void post(void);
    void unpost(void);
    void upCursor(void);
    void downCursor(void);
    void setMenuItems(std::vector<MenuItem*>& menuItems);
    MenuItem* getCurrentItme(void);

  protected:
    ITEM** _itemList;
    ITEM* _currentItem;
    MENU* _menu;
    std::vector<MenuItem*> _menuItems;

};

#endif /* __MENU_WINDOW_H__ */
