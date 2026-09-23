#include <numeric>

#define SDL_MAIN_HANDLED
#include "../Include/UI.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  (void)argv;
  (void)argc;
  UI ui;
  if (ui.Init_Window()) {
    ui.Run();
  } else {
    std::cerr << "Application failed to initialize." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
