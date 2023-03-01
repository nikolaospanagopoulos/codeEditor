#pragma once
#include "EditorRow.hpp"
#include <termios.h>
#include <vector>
class terminalState {
public:
  struct termios original;
  int cursorX, cursorY;
  int rows;
  int columns;
  int rowOffset; // row where the user scrolled to
  int numRows;
  std::vector<EditorRow *> *editorRows;
  terminalState();
  ~terminalState();
};
