#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../include/Game.hpp"

namespace pacman {

Game::Game() {
  sf::VideoMode mode({SCREEN_WIDTH, SCREEN_HEIGHT});
  window.create(mode, SCREEN_TITLE.data(), sf::Style::Default);

  if (!window.isOpen()) {
    throw std::runtime_error("Error opening window");
  }

  window.setFramerateLimit(TARGET_FPS);
  window.setVerticalSyncEnabled(true);

  sf::Vector2i desktop = sf::Vector2i(sf::VideoMode::getDesktopMode().size);
  sf::Vector2i windowPos((desktop.x - static_cast<int>(mode.size.x)) / 2,
                         (desktop.y - static_cast<int>(mode.size.y)) / 2);
  window.setPosition(windowPos);

  pacman.setPosition(map.getPacmanStartPos());
  ghosts.resetPositions(map.getGhostStartPositions());

  gameState = GameState::MENU;
}

Game::~Game() { window.close(); }

void Game::loadGameResources() {
  try {
    entity.setWindowIcon(window);
  } catch (const std::exception &) {
    std::cerr << "Warning: (no icon)" << std::endl;
  }
  try {
    pacman.loadTextures();
  } catch (const std::exception &) {
    throw;
  }
  try {
    ghosts.loadTextures();
  } catch (const std::exception &) {
    throw;
  }
  try {
    entity.loadMenuMusic();
    entity.playMenuMusic();
  } catch (const std::exception &) {
    std::cerr << "Warning: failed to load music" << std::endl;
  }
  try {
    entity.loadPelletSound();
  } catch (const std::exception &) {
    std::cerr << "Game will run without pellet sound" << std::endl;
  }
  try {
    entity.loadPowerPelletSound();
  } catch (const std::exception &) {
    std::cerr << "Game will run without power pellet sound" << std::endl;
  }
  try {
    entity.loadDeathSound();
  } catch (const std::exception &) {
    std::cerr << "Game will run without death sound" << std::endl;
  }
  try {
    entity.loadFonts();
  } catch (const std::exception &) {
    std::cerr << "Warning: failed to load font" << std::endl;
  }
}

void Game::run() {
  bool running = true;
  sf::Clock deltaClock;

  loadGameResources();

  while (window.isOpen() && running) {
    while (const std::optional event = window.pollEvent()) {
      processEvents(event, running);
    }

    sf::Time deltaTime = deltaClock.restart();
    float dt = deltaTime.asSeconds();

    render(running);

    if (windowClock.getElapsedTime().asSeconds() >= 3.5f) {
      if (gameState == GameState::PLAYING &&
          ghosts.checkPacmanCollision(pacman.getSprite()) &&
          windowClock.getElapsedTime().asSeconds() >= 1.5f) {
        gameState = GameState::RESET;
        pacman.setDirection(Direction::RIGHT);
        pacman.loseLife();
        entity.playDeathSound();
        windowClock.restart();
      }
      update(dt);
      if (gameState == GameState::RESET) {
        pacman.setPosition(map.getPacmanStartPos());
        ghosts.resetPositions(map.getGhostStartPositions());
        loseTriggered = false;
        gameState = GameState::PLAYING;
      }
    }
  }

  utils.writeScore(gameState);
}

void Game::resetGame() {
  map.resetMap();
  entity.playMenuMusic();
  pacman.resetLives();
  pacman.resetAnimation();
  pacman.setPosition(map.getPacmanStartPos());
  pacman.setDirection(Direction::RIGHT);
  ghosts.resetPositions(map.getGhostStartPositions());
  ghosts.resetState();
  Utils::resetScore();
  Utils::resetChoice();
  loseTriggered = false;
  gameState = GameState::MENU;
  windowClock.restart();
  endClock.restart();
}

void Game::processEvents(const std::optional<sf::Event> event, bool &running) {
  if (!event) {
    return;
  }
  switch (gameState) {
  case GameState::VICTORY:
  case GameState::LOSE:
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::R)) {
      resetGame();
    }
    break;
  default:
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape)) {
      gameState = GameState::MENU;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q)) {
      running = false;
      windowClock.restart();
    }
    break;
  }
  if (event->is<sf::Event::Closed>()) {
    window.close();
    windowClock.restart();
  }
}

void Game::update(float dt) {
  sf::Vector2f oldPacmanPos = pacman.getPosition();
  std::vector<sf::Vector2f> oldGhostPositions(MAX_GHOSTS);
  for (unsigned int i = 0; i < MAX_GHOSTS; i++)
    oldGhostPositions[i] = ghosts.getPosition(i);

  pacman.update(dt, TARGET_FPS, gameState);
  ghosts.update(map, pacman.getPosition(), pacman.getDirection(), dt,
                TARGET_FPS, gameState);

  if (map.checkWallCollision(pacman.getSprite())) {
    pacman.setPosition(oldPacmanPos);
  }
  for (unsigned int i = 0; i < MAX_GHOSTS; i++) {
    if (map.checkWallCollision(ghosts.getSprite(i))) {
      ghosts.setPosition(i, oldGhostPositions[i]);
    }
  }

  int consumed = map.handlePelletCollision(pacman.getSprite());
  if (consumed == 1) {
    entity.playPelletSound();
    utils.consumePellet();
  } else if (consumed == 2) {
    entity.playPowerPelletSound();
    utils.consumePowerPellet();
    ghosts.frighten();
  }

  if (gameState == GameState::PLAYING && map.allPelletsEaten()) {
    gameState = GameState::VICTORY;
    endClock.restart();
  }

  if (gameState == GameState::PLAYING && pacman.getLives() <= 0 &&
      !loseTriggered) {
    gameState = GameState::LOSE;
    loseTriggered = true;
    endClock.restart();
  }
}

void Game::render(bool &running) {
  window.clear(sf::Color::Black);
  map.render(window);
  pacman.render(window);
  ghosts.render(window);
  entity.drawUI(window, gameState, utils, running, pacman.getLives());
  stateHelper.drawGameStatus(window, gameState, entity.getWinText(),
                             entity.getLoseText());
  window.display();
}

}
