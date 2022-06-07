#ifndef __ADDRDUMP_TEXT_EDITOR_H__
#define __ADDRDUMP_TEXT_EDITOR_H__

#include <string>

#include <ncurses.h>
#include <form.h>

#include "window.h"

class AddrDumpTextEditor : public Window {
  public:
    AddrDumpTextEditor() = delete;
    AddrDumpTextEditor(Point start = {0, 0}, Point end = {0, 0});
    ~AddrDumpTextEditor();
    void show(void);
    std::string editText(void);

  private:
    bool isValidAddress(std::string address);
};

#endif /* __ADDRDUMP_TEXT_EDITOR_H__ */
