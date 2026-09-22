#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace pacman {

enum class GameState {
  MENU,
  PLAYING,
  VICTORY,
  LOSE,
  RESET
};

struct State {
  static constexpr const char *Menu = "MENU";
  static constexpr const char *Playing = "PLAYING";
  static constexpr const char *Victory = "VICTORY";
  static constexpr const char *Lose = "LOSE";
  static constexpr const char *Reset = "RESET";

  static const std::string getGameState(GameState gameState) {
    switch (gameState) {
    case GameState::MENU:
      return Menu;
    case GameState::PLAYING:
      return Playing;
    case GameState::VICTORY:
      return Victory;
    case GameState::LOSE:
      return Lose;
    case GameState::RESET:
      return Reset;
    default:
      return "Unknown";
    }
  }
  void drawGameStatus(sf::RenderWindow &window, GameState &gameState,
                      sf::Text &winText, sf::Text &loseText) {
    if (gameState == GameState::VICTORY) {
      winText.setPosition({370, 1010});
      window.draw(winText);
    } else if (gameState == GameState::LOSE) {
      loseText.setPosition({275, 1010});
      window.draw(loseText);
    }
  }
};

}

#endif
