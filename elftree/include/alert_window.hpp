#ifndef __ALERT_WINDOW_HPP__
#define __ALERT_WINDOW_HPP__

#include <string>

#include <ncurses.h>
#include <unistd.h>

class Alert {
  public:
    static void notify(std::string message) {
      WINDOW* _window;
      int maxX, maxY;
      int startX, startY;

      getmaxyx(stdscr, maxY, maxX);
      startX = (maxX / 2) - message.size();
      startY = (maxY / 2) - 1;

      curs_set(0);
      _window = newwin(3, message.size() + 4, startY, startX);
      mvwprintw(_window, 1, 2, "%s", message.c_str());
      box(_window, 0, 0);
      wrefresh(_window);
      usleep(500000);
      wborder(_window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
      wrefresh(_window);
      delwin(_window);
    }
};

#endif /* __ALERT_WINDOW_HPP__ */
