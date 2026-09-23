#include <cstdlib>

#include <iostream>

#include "../include/MP.hpp"
#include "../include/UI.hpp"

int main() {
  UI ui;
  std::string fileName = ui.Menu();

  if (fileName.empty()) {
    return EXIT_FAILURE;
  }

  MPlayer mp;
  mp.LoadAndPlay(fileName);
  mp.Run();

  return EXIT_SUCCESS;
}
