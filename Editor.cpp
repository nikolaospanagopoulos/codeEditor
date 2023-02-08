#include "Editor.hpp"
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#define CTRL_KEY(key) ((key)&0b00011111)
#include "CustomException.hpp"
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>
Editor::terminalSettings::terminalSettings() : rows{0}, columns{0} {}

// get terminal rows and columns
void Editor::terminalSettings::getWindowSize() {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    throw CustomException(
        (char *)"There was a problem calculating the terminal dimentions");
  }
  rows = ws.ws_col;
  columns = ws.ws_row;
}

// terminal escape sequence
void Editor::refreshScreen() {

  // We are using the J command (Erase In Display) to clear the screen.
  // 2 means to clear whole screen
  write(STDOUT_FILENO, "\x1b[2J]", 4);
  // put cursor at the beggining
  write(STDOUT_FILENO, "\x1b[H", 3);

  drawRaws();

  // put cursor at the beggining
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void Editor::drawRaws() {
  for (int i{}; i < settings.rows; i++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

void Editor::clearScreen() {
  write(STDOUT_FILENO, "\x1b[2J]", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void Editor::readKeyPress() {
  char c{};
  while (true) {
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
Editor::Editor() : enteredRawMode{false}, settings{} {}

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
}
