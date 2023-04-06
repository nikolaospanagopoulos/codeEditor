#pragma once
#include "EditorRow.hpp"
#include "terminalState.hpp"
#include <iostream>
#include <sstream>
#include <string>

class Editor {
private:
  enum editorSpecialKey {
    BACKSPACE = 127,
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
  std::string *rowsToString();
  void deleteRow(int posision);
  void appendString(EditorRow *row, std::string *toAppend);
  void deleteChar();
  void deleteRowChar(EditorRow *row);
  void save();
  void drawStatusBar();
  void drawRows();
  bool enteredRawMode;
  int getDirectionKeys(const char &c) const;
  int getArrowKeys() const;
  void insertNewLine();
  void appendRow(int at, const std::string &row);
  void scroll();
  void getCusrorPosition();
  void updateRow(EditorRow *row);
  int cursorXtoRx(EditorRow *row);
  void drawStatusMessageBar();
  void editorRowInsertChar(EditorRow *row, int at, int c);
  void insertChar(int c);

public:
  template <class... T>
  std::string setStatusMessage(std::string message, T... args) {
    std::stringstream stream;
    stream << message << " ";
    using List = int[];
    (void)List{0, ((void)(stream << args << " "), 0)...};
    state.statusMessageTime = time(NULL);
    state.statusMessage = stream.str();
    return stream.str();
  }
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
