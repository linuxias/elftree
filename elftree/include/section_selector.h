#ifndef __SECTION_SELECTOR_H__
#define __SECTION_WINDOW_H__

#include <string>
#include <vector>

#include "window.h"
#include "menu_window.h"

class SectionSelector : public MenuWindow {
  public:
    SectionSelector(Point start = {0, 0}, Point end = {0, 0});
    ~SectionSelector();
    void post(void);
    MenuItem* selectItem(void);
};

#endif /* __SECTION_SELECTOR_H__ */

