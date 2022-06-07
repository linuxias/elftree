#include "menu_window.h"

MenuWindow::MenuWindow(Point start, Point end) : Window(start, end)
{
  _itemList = nullptr;
  _currentItem = nullptr;
  _menu = nullptr;
}

MenuWindow::~MenuWindow()
{
  unpost();
  if (_window)
    delwin(_window);
}

void MenuWindow::post(void) {}

void MenuWindow::unpost(void)
{
  int size = _menuItems.size() + 1;

  if (_menu) {
    unpost_menu(_menu);
    free_menu(_menu);
    _menu = nullptr;
  }

  if (_itemList) {
    for (int i = 0; i < size; i++) {
      if (_itemList[i]) {
        free_item(_itemList[i]);
        _itemList[i] = nullptr;
      }
    }
  }

  _menuItems.clear();
}

void MenuWindow::upCursor(void)
{
  menu_driver(_menu, REQ_UP_ITEM);
  _currentItem = current_item(_menu);
}

void MenuWindow::downCursor(void)
{
  menu_driver(_menu, REQ_DOWN_ITEM);
  _currentItem = current_item(_menu);
}

void MenuWindow::setMenuItems(std::vector<MenuItem*>& menuItems)
{
  _menuItems = menuItems;
}
