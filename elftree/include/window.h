#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <ncurses.h>
#include <form.h>

typedef struct {
  int x;
  int y;
} Point;

class Window {
  public:
    Window() = delete;
    Window(Point start = {0, 0}, Point end = {0, 0});
    virtual ~Window(void);
    void refresh(void);
    void reset(void);
    void showBox(void);
    void hideBox(void);
    static Point getMaxXY(void);

  protected:
    Point _startPoint;
    Point _endPoint;
    WINDOW* _window;
};

#endif /* __WINDOW_H__ */
