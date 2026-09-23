#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../include/Entity.hpp"

namespace pacman {

Entity::Entity()
    : pelletSound(pelletBuffer), powerPelletSound(powerPelletBuffer),
      deathSound(deathBuffer), scoreText(scoreFont), livesText(livesFont),
      winText(winFont), loseText(loseFont), restartText(restartFont) {
  menuMusic.setVolume(30.f);
  pelletSound.setVolume(30.f);
  menuFont.setSmooth(true);

  menuTexts = std::vector<sf::Text>(MAX_TEXT, sf::Text(menuFont));

  menuTexts[0].setString("PACMAN");
  menuTexts[1].setString("Play");
  menuTexts[2].setString("Exit");

  menuTexts[0].setPosition({330.f, 150.f});
  menuTexts[1].setPosition({370.f, 350.f});
  menuTexts[2].setPosition({370.f, 550.f});

  menuTexts[0].setFillColor(sf::Color::Green);

  for (auto &t : menuTexts)
    t.setCharacterSize(40);

  scoreText.setFillColor(sf::Color::White);
  scoreText.setStyle(sf::Text::Bold);
  scoreText.setCharacterSize(30);
  scoreText.setPosition({50, 1010});

  livesText.setFillColor(sf::Color::White);
  livesText.setStyle(sf::Text::Bold);
  livesText.setCharacterSize(30);
  livesText.setPosition({670, 1010});

  restartText.setPosition({150, 500});

  winText.setString("YOU WIN");
  loseText.setString("YOU LOSE GAME OVER");
  restartText.setString("Press r or <ENTER> to restart");

  winText.setFillColor(sf::Color::Green);
  loseText.setFillColor(sf::Color::Red);
  restartText.setFillColor(sf::Color::White);

  winText.setStyle(sf::Text::Bold);
  loseText.setStyle(sf::Text::Bold);
  restartText.setStyle(sf::Text::Bold);

  winText.setCharacterSize(30);
  loseText.setCharacterSize(30);
  restartText.setCharacterSize(30);
}

Entity::~Entity() {}

void Entity::setWindowIcon(sf::RenderWindow &window) {
  if (!icon.loadFromFile(iconPath))
    throw std::runtime_error("Failed to load window icon");
  window.setIcon(icon.getSize(), icon.getPixelsPtr());
}

bool Entity::loadMenuMusic() {
  if (!menuMusic.openFromFile(menuMusicPath.c_str())) {
    throw std::runtime_error("Error playing music");
  }
  return true;
}

void Entity::playMenuMusic() { menuMusic.play(); }

bool Entity::loadPelletSound() {
  if (!pelletBuffer.loadFromFile(pelletSoundPath)) {
    throw std::runtime_error("Error loading pellet sound");
  }
  return true;
}

bool Entity::loadPowerPelletSound() {
  if (!powerPelletBuffer.loadFromFile(powerPelletSoundPath)) {
    throw std::runtime_error("Error loading power pellet sound");
  }
  return true;
}

void Entity::playPelletSound() {
  if (pelletSound.getStatus() != sf::Sound::Status::Playing) {
    pelletSound.play();
  }
}

void Entity::playPowerPelletSound() {
  if (powerPelletSound.getStatus() != sf::Sound::Status::Playing) {
    powerPelletSound.play();
  }
}

bool Entity::loadDeathSound() {
  if (!deathBuffer.loadFromFile(deathSoundPath)) {
    throw std::runtime_error("Error loading death sound");
  }
  return true;
}

void Entity::playDeathSound() {
  if (deathSound.getStatus() != sf::Sound::Status::Playing) {
    deathSound.play();
  }
}

bool Entity::loadFonts() {
  if (!menuFont.openFromFile(gameFontPath)) {
    throw std::runtime_error("Error loading the menu game font");
  }
  if (!scoreFont.openFromFile(fontPath)) {
    throw std::runtime_error("Error loading the score game font");
  }
  if (!livesFont.openFromFile(fontPath)) {
    throw std::runtime_error("Error loading the lives game font");
  }
  if (!winFont.openFromFile(fontPath)) {
    throw std::runtime_error("Error loading the win game font");
  }
  if (!loseFont.openFromFile(fontPath)) {
    throw std::runtime_error("Error loading the lose game font");
  }
  if (!restartFont.openFromFile(fontPath)) {
    throw std::runtime_error("Error loading the restart game font");
  }
  return true;
}

void Entity::drawUI(sf::RenderWindow &window, GameState &gameState,
                    Utils &utils, bool &running, int pacmanLives) {
  std::stringstream ss;
  ss << "Score : " << Utils::getScore();
  scoreText.setString(ss.str());
  window.draw(scoreText);

  if (gameState == GameState::LOSE || gameState == GameState::VICTORY) {
    window.draw(restartText);
  }

  ss.str("");
  ss << "Lives : " << pacmanLives;
  livesText.setString(ss.str());
  window.draw(livesText);

  utils.handleMenuSelection(window, gameState, running, menuTexts.data());
}

} // namespace pacman
