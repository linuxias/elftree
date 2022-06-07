#include <exception>
#include <string>
#include <vector>

#include "window.h"
#include "info_window.h"

InfoWindow::InfoWindow(Point start, Point end) : Window(start, end)
{
  _startLine = 1;
  createWindow();
}

InfoWindow::~InfoWindow()
{
  delwin(_window);
}

void InfoWindow::createWindow(void)
{
  _window = newwin(_endPoint.y, _endPoint.x,
      _startPoint.y, _startPoint.x);
  scrollok(_window, TRUE);
  box(_window, 0, 0);
  wrefresh(_window);
}

std::vector<std::string> splitStringByLine(std::string s) {
  std::vector<std::string> v;
  int start = 0;
  int d = s.find("\n");
  while (d != -1){
    v.push_back(s.substr(start, d - start));
    start = d + 1;
    d = s.find("\n", start);
  }
  v.push_back(s.substr(start, d - start));

  return v;
}

void InfoWindow::printText(const std::string& text, int start_line)
{
  wmove(_window, 1, 1);
  wclear(_window);
  showBox();

  _information = text;
  _startLine = start_line;

  for (auto& str : splitStringByLine(_information))
    mvwprintw(_window, start_line++, 2, "%s", str.c_str());
}

void InfoWindow::upScroll(void)
{
  if (_information.empty())
    return;

  _startLine--;
  printText(_information, _startLine);
}

void InfoWindow::downScroll(void)
{
  if (_information.empty())
    return;

  /*
   * Line 0 is drawn box.
   * Printing information over line 1 means that
   * there is a space between the box line and
   * the printed string.
   * So, set line 1 over line 0,
   */
  _startLine++;
  if (_startLine > 1) {
    _startLine = 1;
    return;
  }

  printText(_information, _startLine);
}
