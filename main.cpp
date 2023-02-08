#include "CustomException.hpp"
#include "Editor.hpp"
#include <exception>
// set upper three bytes of key number to zero -> mirror CTRL functionality
#define CTRL_KEY(key) ((key)&0b00011111)
int main() {

  Editor editor;
  try {
    editor.enableRawMode();
    editor.settings.getWindowSize();
    editor.refreshScreen();
    editor.readKeyPress();

  } catch (CustomException &e) {
    editor.clearScreen();
    std::cerr << e.what() << std::endl;
  } catch (const std::runtime_error &re) {
    // speciffic handling for runtime_error
    editor.clearScreen();
    std::cerr << "Runtime error: " << re.what() << std::endl;
  } catch (std::exception &ex) {
    editor.clearScreen();
    std::cerr << ex.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception. Could be memory corruption \n";
    editor.clearScreen();
  }

  return 0;
}
