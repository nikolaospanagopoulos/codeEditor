#include "terminalState.hpp"
terminalState::terminalState()
    : cursorX{0}, cursorY{0}, rows{0}, columns{0}, rowOffset{0},
      collumnOffset{0}, numRows{0}, rx{0}, filename{""} {
  editorRows = new std::vector<EditorRow *>{};
}
terminalState::~terminalState() {

  for (size_t i{}; i < editorRows->size(); i++) {
    delete (*editorRows)[i];
  }

  delete editorRows;
}
