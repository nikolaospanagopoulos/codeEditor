#include "Editor.hpp"
#include <cstddef>
#include <exception>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#define CTRL_KEY(key) ((key)&0b00011111)
#include "CustomException.hpp"
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>
Editor::terminalState::terminalState()
    : cursorX{0}, cursorY{0}, rows{0}, columns{0} {}

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
  settings.rows = ws.ws_row;
  settings.columns = ws.ws_col;
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
  is >> settings.rows >> settings.columns;
}

// terminal escape sequence
void Editor::refreshScreen() {
  // make cursor invisible
  buffer->append("\x1b[?25l");
  buffer->append("\x1b[H");

  drawRaws();

  // put cursor at the beggining

  std::stringstream is{};

  // put cursor at the beggining
  // add one to start from 1 based index
  is << "\x1b[" << settings.cursorY + 1 << ";" << settings.cursorX + 1 << "H";

  buffer->append(is.str());

  // make cursor visible
  // put cursor at the beggining
  buffer->append("\x1b[?25h");
  write(STDOUT_FILENO, buffer->c_str(), buffer->size());
}

void Editor::drawRaws() {
  for (int i{}; i < settings.rows; i++) {

    if (i == settings.rows / 3) {
      std::string welcomeMessage{"Greek C++ editor"};
      while (welcomeMessage.size() > (size_t)settings.columns) {
        welcomeMessage.pop_back();
      }

      int padding = (settings.columns - welcomeMessage.size()) / 2;
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

    buffer->append("\x1b[K");
    if (i < settings.rows - 1) {
      buffer->append("\r\n");
    }
  }
}

void Editor::clearScreen() {
  write(STDOUT_FILENO, "\x1b[2J]", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

char Editor::readKeyPress() {
  char c{};
  while (read(STDIN_FILENO, &c, 1) == -1) {
    throw CustomException((char *)"There was a problem reading the input");
  };
  if (c == '\x1b') {
    return getArrowKeys();
  }
  return c;
}

char Editor::getArrowKeys() const {
  char seq[3]{};
  if (read(STDIN_FILENO, &seq[0], 1) != 1)
    return '\x1b';
  if (read(STDIN_FILENO, &seq[1], 1) != 1)
    return '\x1b';
  if (seq[0] == '[') {
    switch (seq[1]) {
    case 'A':
      return 'k';
    case 'B':
      return 'j';
    case 'C':
      return 'l';
    case 'D':
      return 'h';
    }
  }
  return '\x1b';
}

void Editor::processKeypress() {

  char c = readKeyPress();
  switch (c) {
  case CTRL_KEY('q'):
    clearScreen();
    terminate = true;
    break;
  case 'j':
  case 'k':
  case 'l':
  case 'h':
    editorMoveCursor(c);
    break;
  }
}
void Editor::disableRawMode() {

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings.original) == -1) {
    throw CustomException(
        (char *)"There was an error setting the terminal attributes");
  }
}
Editor::Editor() : enteredRawMode{false}, settings{} {

  buffer = new std::string{};
}

void Editor::enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &settings.original) == -1) {
    throw CustomException((char *)"There was an error getting the terminal "
                                  "attributes");
  }
  struct termios raw = settings.original;
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

void Editor::editorMoveCursor(const char &key) {
  switch (key) {
  case 'h':
    if (settings.cursorX > 0) {
      settings.cursorX--;
    }
    break;
  case 'l':
    if (settings.cursorX < settings.columns - 1) {
      settings.cursorX++;
    }
    break;
  case 'j':
    if (settings.cursorY < settings.rows - 1) {
      settings.cursorY++;
    }
    break;
  case 'k':
    if (settings.cursorY > 0) {
      settings.cursorY--;
    }
    break;
  }
}
