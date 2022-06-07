#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <form.h>
#include <menu.h>

#include "window.h"
#include "section_selector.h"
#include "menu_window.h"

SectionSelector::SectionSelector(Point start, Point end) : MenuWindow(start, end)
{
}

SectionSelector::~SectionSelector()
{
}

void SectionSelector::post()
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
  _window = newwin(_endPoint.y - _startPoint.y, _endPoint.x - _startPoint.x, _startPoint.y, _startPoint.x);
  keypad(_window, TRUE);

  set_menu_win(_menu, _window);
  set_menu_sub(_menu, derwin(_window, 0, 0, 1, 1));

  /*
   * The display_row variable is the number of columns
   * in the menu displayed in the section_selector window.
   * Further values can be checked by scrolling.
   * -2 is value about box line.
   */
  int display_row = _endPoint.y - _startPoint.y - 2;
  set_menu_format(_menu, display_row, 1);

  /* Set menu mark to the string " * " */
  set_menu_mark(_menu, " * ");

  post_menu(_menu);
  box(_window, 0, 0);
  wrefresh(_window);
}

MenuItem* SectionSelector::selectItem(void)
{
  int c;
  int index;
  while ((c = wgetch(_window)) != KEY_F(1)) {
    switch(c) {
      case KEY_DOWN:
        downCursor();
        break;
      case KEY_UP:
        upCursor();
        break;
      case 10: // Enter Keycode
        _currentItem = current_item(_menu);
        index = item_index(_currentItem);
        return _menuItems[index];
      case 113: // q - quit
        return nullptr;
    }
  }

  return nullptr;
}
