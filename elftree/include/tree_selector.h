#ifndef __TREE_SELECTOR_H__
#define __TREE_SELECTOR_H__

#include <string>
#include <vector>

#include "window.h"
#include "menu_window.h"

class TreeSelector : public MenuWindow {
  public:
    TreeSelector() = delete;
    TreeSelector(Point start = {0, 0}, Point end = {0 ,0});
    ~TreeSelector();
    void post(void);
    void setCursorPosition(int index);
    int getCursorPosition(void);
    MenuItem* getCurrentItem(void);
    void selectItem(void);
    MenuItem* getSelectedItem(void);

  private:
    ITEM* _selectedItem;
};

#endif /* __TREE_SELECTOR_H__ */
