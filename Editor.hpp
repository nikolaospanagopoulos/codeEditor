#pragma once
#include <bitset>
#include <cctype>
#include <iostream>
#include <termios.h>

class Editor {
private:
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

public:
  char getArrowKeys() const;
  bool terminate = false;
  void editorMoveCursor(const char &key);
  void getWindowSize();
  void getCusrorPosition();
  terminalState settings;
  void clearScreen();
  char readKeyPress();
  void disableRawMode();
  void refreshScreen();
  void enableRawMode();
  void processKeypress();
  ~Editor();
  Editor();
};
