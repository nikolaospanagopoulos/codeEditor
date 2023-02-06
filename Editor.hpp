#pragma once
#include <bitset>
#include <cctype>
#include <iostream>
#include <termios.h>

class Editor {
private:
  void drawRaws();

public:
  typedef struct terminalSettings {
    struct termios original;
  } terminalSettings;
  terminalSettings settings;
  void clearScreen();
  void readKeyPress();
  void disableRawMode();
  void refreshScreen();
  void enableRawMode();
  bool processKeypress(const char &c);
  ~Editor();
  Editor();
};
