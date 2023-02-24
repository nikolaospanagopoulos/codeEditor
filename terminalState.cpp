#include "terminalState.hpp"
terminalState::terminalState()
    : cursorX{0}, cursorY{0}, rows{0}, columns{0}, numRows{0} {
  editorRows = new std::vector<EditorRow *>{};
}
terminalState::~terminalState() {

  for (size_t i{}; i < editorRows->size(); i++) {
    delete (*editorRows)[i];
  }

  delete editorRows;
}
