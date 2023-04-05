#include "CustomException.hpp"
#include "Editor.hpp"
#include <exception>
// set upper three bytes of key number to zero -> mirror CTRL functionality
int main(int argc, char *argv[]) {

  Editor editor;
  try {
    editor.enableRawMode();
    editor.getWindowSize();
    // last row will be for status bar
    editor.state.rows -= 2;
    if (argc >= 2) {
      editor.editorOpen(argv[1]);
    }
    editor.setStatusMessage("HELP: CTRL-q=quit", "CTRL-s=save");
    // read key and refreshScreen
    while (!editor.terminate) {
      editor.refreshScreen();
      editor.processKeypress();
    }

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
