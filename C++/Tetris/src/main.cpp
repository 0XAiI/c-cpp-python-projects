#include <iostream>
#include <stdexcept>

#include "../include/Game.hpp"

int main(int argc, char *argv[]) {
  try {
    Game game;
    game.run();
  } catch (const std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
