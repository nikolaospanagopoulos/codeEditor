#include "Editor.hpp"
#include <cstddef>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#define CTRL_KEY(key) ((key)&0b00011111)
#include "CustomException.hpp"
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>
Editor::terminalSettings::terminalSettings() : rows{0}, columns{0} {}

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
  // We are using the J command (Erase In Display) to clear the screen.
  // 2 means to clear whole screen
  buffer->append("\x1b[2J]");
  buffer->append("\x1b[H");
  // put cursor at the beggining

  drawRaws();

  // put cursor at the beggining
  buffer->append("\x1b[H");
  write(STDOUT_FILENO, buffer->c_str(), buffer->size());
}

void Editor::drawRaws() {
  for (int i{}; i < settings.rows; i++) {
    buffer->append("~");
    if (i < settings.rows - 1) {
      buffer->append("\r\n");
    }
  }
}

void Editor::clearScreen() {
  write(STDOUT_FILENO, "\x1b[2J]", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void Editor::readKeyPress() {
  char c{};
  while (true) {
    refreshScreen();
    if (read(STDIN_FILENO, &c, 1) == -1) {
      throw CustomException((char *)"There was a problem reading the input");
    };
    if (processKeypress(c)) {
      break;
    }
  }
}

bool Editor::processKeypress(const char &c) {
  switch (c) {
  case CTRL_KEY('q'):
    clearScreen();
    return true;
  }
  return false;
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
