#pragma once
#include "EditorRow.hpp"
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
  void scroll();
  void getCusrorPosition();
  void updateRow(EditorRow *row);
  int cursorXtoRx(EditorRow *row);

public:
  void getWindowSize();
  void editorOpen(const std::string fileName);
  bool terminate = false;
  void editorMoveCursor(const int &key);
  terminalState state;
  void clearScreen();
  int readKeyPress();
  void disableRawMode();
  void refreshScreen();
  void enableRawMode();
  void processKeypress();
  ~Editor();
  Editor();
};
