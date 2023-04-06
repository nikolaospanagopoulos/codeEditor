#include "terminalState.hpp"
#include <sstream>
terminalState::terminalState()
    : cursorX{0}, cursorY{0}, rows{0}, columns{0}, rowOffset{0},
      collumnOffset{0}, numRows{0}, rx{0}, filename{""}, statusMessageTime{0},
      statusMessage{""}, dirty{0} {
  editorRows = new std::vector<EditorRow *>{};
}
terminalState::~terminalState() {

  for (auto &i : *editorRows) {
    delete i;
  }

  delete editorRows;
}
