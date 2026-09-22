#include <iostream>

#include "../include/Game.hpp"

int main() {
  try {
    pacman::Game game;
    game.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
