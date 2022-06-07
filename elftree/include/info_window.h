#ifndef __INFO_WINDOW_H__
#define __INFO_WINDOW_H__

#include <string>

#include <ncurses.h>
#include <form.h>

#include "window.h"

class InfoWindow : public Window {
  public:
    InfoWindow() = delete;
    InfoWindow(Point start = {0, 0}, Point end = {0, 0});
    ~InfoWindow();
    void printText(const std::string& text, int start_line);
    void upScroll(void);
    void downScroll(void);

  private:
    int _startLine;
    std::string _information;

    void createWindow(void);
};

#endif /* __INFO_WINDOW_H__ */
