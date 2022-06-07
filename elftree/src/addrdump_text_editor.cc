#include <algorithm>
#include <cctype>

#include <boost/algorithm/string.hpp>

#include <string.h>

#include "alert_window.hpp"
#include "addrdump_text_editor.h"

#define ADDRESS_INPUT_STR

AddrDumpTextEditor::AddrDumpTextEditor(Point start, Point end) : Window(start, end)
{
  _window = newwin(end.y - start.y, end.x - start.x,
      start.y, start.x);
  keypad(_window, TRUE);
  noecho();
  box(_window, 0, 0);
}

AddrDumpTextEditor::~AddrDumpTextEditor()
{
  delwin(_window);
}

void AddrDumpTextEditor::show(void)
{
  curs_set(0);
  wrefresh(_window);
}

std::string AddrDumpTextEditor::editText(void)
{
  short ch;
  std::string retStr;
  mvwprintw(_window, 1, 1, "Enter dump address : 0x");
  wrefresh(_window);
  while ((ch = getch()) != KEY_F(1)) {
    switch (ch) {
      case 27: // ESC
        return "";
      case 10: // Enter
        if (isValidAddress(retStr))
          return retStr;
        Alert::notify("Wrong address.");
        retStr.clear();
        break;
      case 127:
      case '\b':
      case KEY_BACKSPACE: // Backspace
        if (retStr.size() > 0)
          retStr.pop_back();
        break;
      default:
        retStr.push_back(ch);
        if (retStr.size() > 15)
          retStr.pop_back();
        break;
    }
    wclear(_window);
    box(_window, 0, 0);
    mvwprintw(_window, 1, 1, "Enter dump address : 0x%s", retStr.c_str());
    wrefresh(_window);
  }
  return retStr;
}

bool AddrDumpTextEditor::isValidAddress(std::string address)
{
  if (std::all_of(address.begin(), address.end(), ::isxdigit))
    return true;
  return false;
}
