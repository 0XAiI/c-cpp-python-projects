#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "GameState.hpp"
#include "Utils.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

enum class Direction { DOWN = 0, LEFT, RIGHT, UP, NONE };

namespace pacman {

class Entity {
public:
  Entity();
  ~Entity();

  void setWindowIcon(sf::RenderWindow &window);

  bool loadMenuMusic();
  void playMenuMusic();

  bool loadPelletSound();
  void playPelletSound();
  bool loadPowerPelletSound();
  void playPowerPelletSound();
  bool loadDeathSound();
  void playDeathSound();

  bool loadFonts();
  void drawUI(sf::RenderWindow &window, GameState &gameState, Utils &utils,
              bool &running, int pacmanLives);

  sf::Text &getWinText() { return winText; }
  sf::Text &getLoseText() { return loseText; }

private:
  sf::Image icon;
  sf::Music menuMusic;
  sf::SoundBuffer pelletBuffer;
  sf::Sound pelletSound;
  sf::SoundBuffer powerPelletBuffer;
  sf::Sound powerPelletSound;
  sf::SoundBuffer deathBuffer;
  sf::Sound deathSound;
  sf::Font menuFont;
  sf::Font scoreFont;
  std::vector<sf::Text> menuTexts;
  sf::Text scoreText;
  sf::Text livesText;
  sf::Font livesFont;
  sf::Text winText;
  sf::Text loseText;
  sf::Font winFont;
  sf::Font loseFont;
  sf::Font restartFont;
  sf::Text restartText;

  const std::string menuMusicPath = "assets/sounds/start_music.wav";
  const std::string pelletSoundPath = "assets/sounds/pacman_chomp.wav";
  const std::string powerPelletSoundPath = "assets/sounds/pacman_eatfruit.wav";
  const std::string deathSoundPath = "assets/sounds/pacman_death.wav";
  const std::string gameFontPath = "assets/fonts/pac-font.ttf";
  const std::string iconPath = "assets/logo/game.png";
  const std::string fontPath = "assets/fonts/Joystix.TTF";
};

}

#endif
