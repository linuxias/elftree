#ifndef __TUI_H__
#define __TUI_H__

#include <string>
#include <vector>

#include "elftree.h"
#include "window.h"
#include "info_window.h"
#include "menu_window.h"
#include "tree_selector.h"

class ElfTreeTUI {
  public:
    ElfTreeTUI();
    void initTerminal(void);
    void clearTerminal(void);
    void run(void);
    void setMenuList(TreeView* view);

  private:
    std::vector<MenuItem*> _menus;
    Point _maxPoint = {0, 0};
    InfoWindow* _infoWindow;
    TreeSelector* _treeSelector;
    TreeView* _menuTreeView;
    void refreshTerminal();
    void convertViewToItems(TreeView* view);
    void travelTree(TreeItem*& item, int depth);
    void toggleMenu(bool fold);
    void printInformation(int start_line);
    void printSection(std::string section);
    void printAddressDump(const std::string& address);
    std::string selectSection(void);
    std::string inputAddress(void);
};

#endif /* __TUI_H__ */
