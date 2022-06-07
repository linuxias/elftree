#include <ncurses.h>
#include <form.h>

#include "window.h"

Window::Window(Point start, Point end) :
  _startPoint(start), _endPoint(end), _window(nullptr)
{
}

Window::~Window(void)
{
  if (_window)
    delwin(_window);
}

void Window::refresh(void)
{
  wrefresh(_window);
}

void Window::reset(void)
{
  wclear(_window);
  wrefresh(_window);
}

void Window::showBox()
{
  box(_window, 0, 0);
  wrefresh(_window);
}

void Window::hideBox()
{
  wborder(_window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  wrefresh(_window);
}

Point Window::getMaxXY(void)
{
  Point maxPoint;
  getmaxyx(stdscr, maxPoint.y, maxPoint.x);
  return maxPoint;
}
