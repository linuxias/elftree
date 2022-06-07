#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <string.h>

#include "menu_window.h"
#include "tree_selector.h"

TreeSelector::TreeSelector(Point start, Point end) : MenuWindow(start, end)
{
  _selectedItem = nullptr;
}

TreeSelector::~TreeSelector()
{
  unpost();
  if (_window)
    delwin(_window);
}

void TreeSelector::setCursorPosition(int index)
{
  int max_count = item_count(_menu);
  if (index < 0 || index >= max_count)
    throw std::invalid_argument("Wrong index : " + std::to_string(index));
  set_current_item(_menu, _itemList[index]);
}

int TreeSelector::getCursorPosition(void)
{
  _currentItem = current_item(_menu);
  int index = item_index(_currentItem);
  return index;
}

void TreeSelector::post()
{
  int size = _menuItems.size() + 1;
  if (size == 0)
    return;

  _itemList = (ITEM **)calloc(size, sizeof(ITEM *));
  if (_itemList == nullptr)
    throw std::bad_alloc();

  for (int i = 0; i < size - 1; i++) {
    MenuItem* temp_item = _menuItems[i];
    const char* item_name = static_cast<const char*>(temp_item->name.c_str());
    _itemList[i] = new_item(strdup(item_name), "");
    if (temp_item->userptr != nullptr)
      set_item_userptr(_itemList[i], temp_item->userptr);
  }

  _menu = new_menu(_itemList);
  _window = newwin(_endPoint.y, _endPoint.x, _startPoint.y, _startPoint.x);
  keypad(_window, TRUE);

  set_menu_win(_menu, _window);
  set_menu_sub(_menu, derwin(_window, 0, 0, 1, 1));
  int display_row = _endPoint.y - _startPoint.y - 2;
  set_menu_format(_menu, display_row, 1);

  /* Set menu mark to the string " * " */
  set_menu_mark(_menu, " * ");

  post_menu(_menu);
  box(_window, 0, 0);
  wrefresh(_window);
}

MenuItem* TreeSelector::getCurrentItem(void)
{
  int index = getCursorPosition();
  return _menuItems[index];
}

void TreeSelector::selectItem(void)
{
  _currentItem = current_item(_menu);
  _selectedItem = _currentItem;
}

MenuItem* TreeSelector::getSelectedItem(void)
{
  int index;
  if (_selectedItem == nullptr)
    return nullptr;
  index = item_index(_selectedItem);

  return _menuItems[index];
}
