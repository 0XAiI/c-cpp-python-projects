#include <SFML/Window/Keyboard.hpp>

#include <fstream>
#include <iostream>

#include "../include/Utils.hpp"

namespace pacman {

Utils::Utils() {}
Utils::~Utils() {}

void Utils::consumePellet() { score++; }

void Utils::consumePowerPellet() { score += 5; }

void Utils::resetScore() { score = 0; }

void Utils::resetChoice() { choice = 1; }

void Utils::writeScore(GameState &gameState) {
  std::ofstream dataFile(scoreFilePath);
  if (dataFile.is_open()) {
    dataFile << "player Score : " << score << std::endl;
    dataFile << "Game Status : " << State::getGameState(gameState) << std::endl;
  }
}

static inline void drawText(sf::RenderWindow &window, sf::Text text[MAX_TEXT]) {
  for (int i = 0; i < MAX_TEXT; i++) {
    window.draw(text[i]);
  }
}

void Utils::handleMenuSelection(sf::RenderWindow &window, GameState &gameState,
                                bool &running, sf::Text text[MAX_TEXT]) {
  if (gameState != GameState::MENU)
    return;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))
    choice--;
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))
    choice++;

  if (choice < 1)
    choice = 1;
  if (choice > 2)
    choice = 2;

  switch (choice) {
  case 1:
    text[0].setFillColor(sf::Color::White);
    text[1].setFillColor(sf::Color::Green);
    text[2].setFillColor(sf::Color::White);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter))
      gameState = GameState::PLAYING;
    break;
  case 2:
    text[0].setFillColor(sf::Color::White);
    text[1].setFillColor(sf::Color::White);
    text[2].setFillColor(sf::Color::Green);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter))
      running = false;
    break;
  default:
    break;
  }
  drawText(window, text);
}

} // namespace pacman
