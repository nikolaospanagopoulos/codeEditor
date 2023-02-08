#pragma once
#include <bitset>
#include <cctype>
#include <iostream>
#include <termios.h>

class Editor {
private:
  typedef struct terminalSettings {
    struct termios original;
    int rows;
    int columns;

    void getWindowSize();
    terminalSettings();

  } terminalSettings;
  void drawRaws();
  bool enteredRawMode;

public:
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
