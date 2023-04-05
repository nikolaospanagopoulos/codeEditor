#pragma once
#include "EditorRow.hpp"
#include <ctime>
#include <termios.h>
#include <vector>
class terminalState {
public:
  struct termios original;
  int cursorX, cursorY;
  int rows;
  int columns;
  int rowOffset; // row where the user scrolled to
  int collumnOffset;
  int numRows;
  int rx;
  std::string filename;
  time_t statusMessageTime;
  std::string statusMessage;

  std::vector<EditorRow *> *editorRows;
  terminalState();
  ~terminalState();
};
