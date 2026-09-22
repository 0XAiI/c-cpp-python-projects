#ifndef GAME_HPP
#define GAME_HPP

#include "Entity.hpp"
#include "GameState.hpp"
#include "Ghost.hpp"
#include "Map.hpp"
#include "Pacman.hpp"
#include "Utils.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <string_view>

namespace pacman {

class Game {
public:
  Game();
  ~Game();

  constexpr int getWindowHeight() const { return SCREEN_HEIGHT; }
  constexpr int getWindowWidth() const { return SCREEN_WIDTH; }

  void run();

private:
  Map map;
  Entity entity;
  Pacman pacman;
  GameState gameState;
  Utils utils;
  Ghost ghosts;
  State stateHelper;

  bool loseTriggered = false;

  static constexpr int SCREEN_HEIGHT = 1078;
  static constexpr int SCREEN_WIDTH = 896;
  static constexpr std::string_view SCREEN_TITLE = "PACMAN GAME";
  static constexpr int TARGET_FPS = 60;

  sf::RenderWindow window;
  sf::Clock windowClock;
  sf::Clock endClock;

  void processEvents(const std::optional<sf::Event> event, bool &running);
  void update(float dt);
  void render(bool &running);
  void resetGame();
  void loadGameResources();
};

}

#endif
