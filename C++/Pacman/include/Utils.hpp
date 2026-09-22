#ifndef UTILS_HPP
#define UTILS_HPP

#include "GameState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#define MAX_TEXT 3

namespace pacman {

class Utils {
public:
  Utils();
  ~Utils();

  void consumePellet();
  void consumePowerPellet();
  void writeScore(GameState &gameState);
  void handleMenuSelection(sf::RenderWindow &window, GameState &gameState,
                           bool &running, sf::Text text[MAX_TEXT]);

  static inline int getScore() { return score; }
  static void resetScore();
  static void resetChoice();

private:
  static inline int score = 0;
  static inline int choice = 1;
  std::string scoreFilePath = "assets/data/data.txt";
};

}

#endif
