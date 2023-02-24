#pragma once
#include "terminalState.hpp"
#include <iostream>

class Editor {
private:
  enum editorSpecialKey {
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    DELETE
  };
  // cleanup
  std::string *buffer;
  void drawRaws();
  bool enteredRawMode;
  int getDirectionKeys(const char &c) const;
  int getArrowKeys() const;
  void appendRow(const std::string &row);

public:
  void editorOpen(const std::string fileName);
  bool terminate = false;
  void editorMoveCursor(const int &key);
  void getWindowSize();
  void getCusrorPosition();
  terminalState settings;
  void clearScreen();
  int readKeyPress();
  void disableRawMode();
  void refreshScreen();
  void enableRawMode();
  void processKeypress();
  ~Editor();
  Editor();
};
