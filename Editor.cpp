#include "Editor.hpp"
#include "EditorRow.hpp"
#include <cstddef>
#include <cstring>
#include <exception>
#include <istream>
#include <stdexcept>
#define CTRL_KEY(key) ((key)&0b00011111)
#include "CustomException.hpp"
#include <cstdlib>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>

void Editor::appendString(EditorRow *row, std::string *toAppend) {
  // maybe reserve space
  *(row->rowText) += *toAppend;
  row->size += toAppend->size();
  updateRow(row);
  state.dirty++;
}
void Editor::deleteRow(int posision) {
  if (posision < 0 || posision >= state.numRows) {
    return;
  }
  // maybe decrement numrows by one ????
  delete state.editorRows->at(posision);
  state.editorRows->erase(state.editorRows->begin() + posision);
  state.numRows--;
  state.dirty++;
}
void Editor::updateRow(EditorRow *row) {
  row->render->clear();
  int idx = 0;
  for (size_t i{}; i < (size_t)row->size; i++) {
    if (row->rowText->at(i) == '\t') {
      row->render->insert(idx++, 1, ' ');
      while (idx % 8 != 0) {
        row->render->insert(idx++, 1, ' ');
      }
    } else {
      row->render->insert(idx++, 1, row->rowText->at(i));
    }
  }
}
void Editor::appendRow(int at, const std::string &text) {
  if (at < 0 || at > state.numRows)
    return;

  EditorRow *row = new EditorRow{};
  row->rowText->append(text);
  row->size = text.size();
  state.editorRows->insert(state.editorRows->begin() + at, row);

  updateRow(row);
  state.numRows++;
  state.dirty++;
}
std::string *Editor::rowsToString() {
  int totalSize{};
  for (size_t i{}; i < (size_t)state.numRows; i++) {
    totalSize += state.editorRows->at(i)->size + 1;
  }
  std::string *newFile = new std::string{};
  // todo: maybe std::move???
  newFile->reserve(totalSize);
  for (size_t i{}; i < (size_t)state.numRows; i++) {
    newFile->append(state.editorRows->at(i)->rowText->c_str());
    newFile->append("\n");
  }

  return newFile;
}
std::string Editor::prompt(std::string &prompt) {
  std::string userAnswerBuffer{};
  while (1) {
    setStatusMessage(prompt, userAnswerBuffer);
    refreshScreen();
    int c = readKeyPress();
    if (c == BACKSPACE) {
      if (!userAnswerBuffer.empty()) {
        userAnswerBuffer.pop_back();
      }
    } else if (c == '\x1b') {
      setStatusMessage("");
      userAnswerBuffer.clear();
      return "";
    } else if (c == '\r') {
      if (!userAnswerBuffer.empty()) {
        setStatusMessage("");
        return userAnswerBuffer;
      }
    } else if (!std::iscntrl(c) && c < 128) {
      userAnswerBuffer.push_back(c);
    }
  }
}
void Editor::save() {
  if (state.filename.empty()) {
    std::string promptStr{"save as: "};
    state.filename = prompt(promptStr);
    if (state.filename.empty()) {
      setStatusMessage("Save aborted");
      return;
    }
  }
  std::ofstream outFile{state.filename};
  if (!outFile) {
    throw CustomException((char *)"There was an error creating the file");
  }

  std::string *newFileContents = rowsToString();

  int numOfBytes = newFileContents->size();
  if (outFile << *newFileContents) {
    setStatusMessage("writen to disk", numOfBytes, "bytes");
  } else {

    setStatusMessage("couldnt write to disk", "I/O error");
    delete newFileContents;
    outFile.close();
    throw CustomException((char *)"couldnt write to disk");
  }
  state.dirty = 0;
  delete newFileContents;
  outFile.close();
}
void Editor::editorOpen(const std::string fileName) {
  std::ifstream inFile{};
  inFile.open(fileName);
  if (!inFile) {
    throw CustomException((char *)"There was a problem opening the file ");
  }

  state.filename = fileName;
  std::string line{};
  while (std::getline(inFile, line)) {
    appendRow(state.numRows, line);
  }

  inFile.close();
  state.dirty = 0;
}
void Editor::insertNewLine() {
  if (state.cursorX == 0) {
    appendRow(state.cursorY, "");
  } else {
    EditorRow *row = state.editorRows->at(state.cursorY);
    std::string remains = &row->rowText->c_str()[state.cursorX];
    appendRow(state.cursorY + 1, remains);
    row = state.editorRows->at(state.cursorY);
    row->rowText->resize(state.cursorX);
    row->size = row->rowText->size();
    updateRow(row);
  }
  state.cursorY++;
  state.cursorX = 0;
}
void Editor::editorRowInsertChar(EditorRow *row, int at, int c) {
  if (at < 0 || at > row->size) {
    at = row->size;
  }
  row->size++;
  row->rowText->insert(row->rowText->begin() + at, c);
  updateRow(row);
}
void Editor::drawStatusMessageBar() {
  buffer->append("\x1b[K", 3);
  if (!state.statusMessage.empty() &&
      time(NULL) - state.statusMessageTime < 5) {
    buffer->append(state.statusMessage);
  }
}
void Editor::insertChar(int c) {
  if (state.cursorY == state.numRows) {
    // we are at the end of file
    appendRow(state.numRows, "");
  }
  editorRowInsertChar(state.editorRows->at(state.cursorY), state.cursorX, c);
  state.cursorX++;
  state.dirty++;
}
// get terminal rows and columns
void Editor::getWindowSize() {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
      throw CustomException(
          (char *)"There was a problem calculating the terminal dimentions");
    }
    getCusrorPosition();
    return;
  }
  state.rows = ws.ws_row;
  state.columns = ws.ws_col;
}

// get cursor position
void Editor::getCusrorPosition() {
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
    throw CustomException(
        (char *)("There was a problem calculating the cursor position"));
  }
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[')
    return;

  std::string tempString{&buf[2]};
  for (size_t i{}; i < tempString.size(); i++) {
    if (!std::isdigit(tempString[i])) {
      tempString[i] = ' ';
    }
  }
  std::stringstream is{tempString};
  is >> state.rows >> state.columns;
}

int Editor::cursorXtoRx(EditorRow *row) {
  int rx = 0;
  for (size_t i{}; i < (size_t)state.cursorX; i++) {
    if (row->rowText->at(i) == '\t') {
      rx += (8 - 1) - (rx % 8);
    }
    rx++;
  }
  return rx;
}

// terminal escape sequence
void Editor::refreshScreen() {
  scroll();
  // make cursor invisible
  buffer->append("\x1b[?25l");
  buffer->append("\x1b[H");

  drawRows();
  drawStatusBar();
  drawStatusMessageBar();

  // put cursor at the beggining

  std::stringstream is{};

  // put cursor at the beggining
  // add one to start from 1 based index
  is << "\x1b[" << (state.cursorY - state.rowOffset) + 1 << ";"
     << (state.rx - state.collumnOffset) + 1 << "H";

  buffer->append(is.str());

  // make cursor visible
  // put cursor at the beggining
  buffer->append("\x1b[?25h");
  write(STDOUT_FILENO, buffer->c_str(), buffer->size());
  // empty buffer
  buffer->clear();
}

void Editor::drawRows() {
  for (int i{}; i < state.rows; i++) {

    int fileRow = i + state.rowOffset;
    if (fileRow >= state.numRows) {

      if (state.numRows == 0 && i == state.rows / 3) {
        std::string welcomeMessage{"text editor"};
        while (welcomeMessage.size() > (size_t)state.columns) {
          welcomeMessage.pop_back();
        }

        int padding = (state.columns - welcomeMessage.size()) / 2;
        if (padding) {
          buffer->append("~");
          padding--;
        }
        while (padding--) {
          buffer->append(" ");
        }

        buffer->append(welcomeMessage);
      } else {
        buffer->append("~");
      }
    } else {
      // todo: check if screen is wide enough for text.
      int len =
          state.editorRows->at(fileRow)->render->size() - state.collumnOffset;
      if (len < 0)
        len = 0;
      if (len > state.columns) {
        len = state.columns;
      }
      buffer->append(
          &state.editorRows->at(fileRow)->render->c_str()[state.collumnOffset],
          len);
    }

    buffer->append("\x1b[K", 3);
    buffer->append("\r\n", 2);
  }
}
void Editor::drawStatusBar() {
  buffer->append("\x1b[7m", 4);
  std::stringstream infoText{};
  std::string statusFileName =
      !state.filename.empty() ? state.filename : "No Name";
  infoText << statusFileName;
  if (!state.filename.empty()) {
    infoText << " line: " << state.cursorY + 1 << "/" << state.numRows;
  }
  if (state.dirty) {
    infoText << " (mofified)";
  }
  std::string finalStatus = infoText.str();
  int len = finalStatus.size();
  buffer->append(finalStatus, 0, len);
  while (len < state.columns) {
    buffer->append(" ", 1);
    len++;
  }
  buffer->append("\x1b[m", 3);
  buffer->append("\r\n", 2);
}
void Editor::clearScreen() {
  write(STDOUT_FILENO, "\x1b[2J]", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, "\033c", 3);
}

int Editor::readKeyPress() {
  char c{};
  while (read(STDIN_FILENO, &c, 1) == -1) {
    throw CustomException((char *)"There was a problem reading the input");
  };
  if (c == '\x1b') {
    return getArrowKeys();
  }

  // todo: make it better
  return c;
}
int Editor::getDirectionKeys(const char &c) const {

  switch (c) {
  case 'j':
    return Editor::editorSpecialKey::ARROW_DOWN;
  case 'k':
    return Editor::editorSpecialKey::ARROW_UP;
  case 'l':
    return Editor::editorSpecialKey::ARROW_RIGHT;
  case 'h':
    return Editor::editorSpecialKey::ARROW_LEFT;
  }
  return c;
}
int Editor::getArrowKeys() const {
  char seq[3]{};
  if (read(STDIN_FILENO, &seq[0], 1) != 1)
    return '\x1b';
  if (read(STDIN_FILENO, &seq[1], 1) != 1)
    return '\x1b';
  if (seq[0] == '[') {
    if (seq[1] >= '0' && seq[1] <= '9') {
      if (read(STDIN_FILENO, &seq[2], 1) != 1) {
        return '\x1b';
      }
      if (seq[2] == '~') {
        switch (seq[1]) {
        case '1':
          return HOME;
        case '3':
          return DELETE;
        case '4':
          return END;
        case '5':
          return PAGE_UP;
        case '6':
          return PAGE_DOWN;
        case '7':
          return HOME;
        case '8':
          return END;
        }
      }
    } else {
      switch (seq[1]) {
      case 'A':
        return ARROW_UP;
      case 'B':
        return ARROW_DOWN;
      case 'C':
        return ARROW_RIGHT;
      case 'D':
        return ARROW_LEFT;
      case 'H':
        return HOME;
      case 'F':
        return END;
      }
    }
  }
  return '\x1b';
}

void Editor::editorMoveCursor(const int &key) {
  EditorRow *row = (state.cursorY >= state.numRows)
                       ? nullptr
                       : state.editorRows->at(state.cursorY);
  switch (key) {
  case ARROW_LEFT:
    if (state.cursorX > 0) {
      state.cursorX--;
    } else if (state.cursorY > 0) {
      state.cursorY--;
      state.cursorX = state.editorRows->at(state.cursorY)->size;
    }

    break;
  case ARROW_RIGHT:
    if (row && state.cursorX < row->size) {
      state.cursorX++;
    } else if (row && state.cursorX == row->size) {
      state.cursorY++;
      state.cursorX = 0;
    }
    break;
  case ARROW_DOWN:
    if (state.cursorY < state.numRows - 1) {
      state.cursorY++;
    }
    break;
  case ARROW_UP:
    if (state.cursorY > 0) {
      state.cursorY--;
    }
    break;
  }
  row = (state.cursorY >= state.numRows) ? nullptr
                                         : state.editorRows->at(state.cursorY);
  int rowLen = row ? row->size : 0;
  if (state.cursorX > rowLen) {
    state.cursorX = rowLen;
  }
}
void Editor::deleteChar() {
  // maybe minus 1
  if (state.cursorY == state.numRows) {
    return;
  }
  if (state.cursorX == 0 && state.cursorY == 0) {
    return;
  }
  EditorRow *row = state.editorRows->at(state.cursorY);
  if (state.cursorX > 0) {
    deleteRowChar(state.editorRows->at(state.cursorY));
    state.cursorX--;
  } else {
    state.cursorX = state.editorRows->at(state.cursorY - 1)->size;
    // maybe move text
    appendString(state.editorRows->at(state.cursorY - 1),
                 std::move(row->rowText));
    deleteRow(state.cursorY);
    state.cursorY--;
  }
}
void Editor::deleteRowChar(EditorRow *row) {
  row->rowText->erase(row->rowText->begin() + state.cursorX - 1);
  row->size--;
  updateRow(row);
  state.dirty++;
}
void Editor::processKeypress() {
  static int quitTimes = 1;
  int c = readKeyPress();
  switch (c) {
  case CTRL_KEY('q'):
    if (state.dirty && quitTimes > 0) {
      setStatusMessage("WARNING!!!!", "unsaved changes!!!");
      quitTimes--;
      return;
    }
    clearScreen();
    terminate = true;
    break;
  case '\r':
    insertNewLine();
    break;
  case PAGE_UP:
  case PAGE_DOWN: {
    int times = state.rows;
    while (times--) {
      editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }

    break;
  }
  case CTRL_KEY('s'):
    save();
    break;
  case HOME:
    state.cursorX = 0;
    break;
    /*
case END:
state.cursorX = state.columns - 1;
break;
    */
  case END:
    if (state.cursorY < state.numRows) {
      state.cursorX = state.editorRows->at(state.cursorY)->size;
    }
    break;
  case BACKSPACE:
  case CTRL_KEY('h'):
  case DELETE:
    if (c == DELETE) {
      editorMoveCursor(ARROW_RIGHT);
    }
    deleteChar();
    break;
  case Editor::editorSpecialKey::ARROW_UP:
  case Editor::editorSpecialKey::ARROW_DOWN:
  case Editor::editorSpecialKey::ARROW_LEFT:
  case Editor::editorSpecialKey::ARROW_RIGHT:
    /*
case 'j':
case 'k':
case 'l':
case 'h':
*/
    editorMoveCursor(c);
    break;
  case '\0':
    break;
  case CTRL_KEY('l'):
  case '\x1b':
    break;
  default:

    insertChar(c);
    break;
  }
}
void Editor::scroll() {
  state.rx = 0;
  if (state.cursorY < state.numRows) {
    state.rx = cursorXtoRx(state.editorRows->at(state.cursorY));
  }
  if (state.cursorY < state.rowOffset) {
    state.rowOffset = state.cursorY;
  }
  if (state.cursorY >= state.rowOffset + state.rows) {
    state.rowOffset = state.cursorY - state.rows + 1;
  }
  if (state.rx < state.collumnOffset) {
    state.collumnOffset = state.rx;
  }
  if (state.rx >= state.collumnOffset + state.columns) {
    state.collumnOffset = state.rx - state.columns + 1;
  }
}
void Editor::disableRawMode() {

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &state.original) == -1) {
    throw CustomException(
        (char *)"There was an error setting the terminal attributes");
  }
}
Editor::Editor() : enteredRawMode{false}, state{} {

  buffer = new std::string{};
}

void Editor::enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &state.original) == -1) {
    throw CustomException((char *)"There was an error getting the terminal "
                                  "attributes");
  }
  struct termios raw = state.original;
  // stop control-S control-Q
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_lflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  enteredRawMode = true;
}

Editor::~Editor() {
  if (enteredRawMode) {
    disableRawMode();
  }

  delete buffer;
}
