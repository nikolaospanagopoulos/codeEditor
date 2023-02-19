#pragma once
#include <bitset>
#include <cctype>
#include <iostream>
#include <termios.h>

class Editor {
private:
  enum editorSpecialKey {
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    PAGE_UP,
    PAGE_DOWN,
  };
  std::string *buffer;
  typedef struct terminalState {
    struct termios original;
    int cursorX, cursorY;
    int rows;
    int columns;

    terminalState();

  } terminalState;
  void drawRaws();
  bool enteredRawMode;
  int getDirectionKeys(const char &c) const;
  int getArrowKeys() const;

public:
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
