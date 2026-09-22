#ifndef PACMAN_HPP
#define PACMAN_HPP

#include "Entity.hpp"
#include "GameState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace pacman {

class Pacman {
public:
  Pacman();
  ~Pacman();

  void render(sf::RenderWindow &window);
  void update(float deltaTimer, const int FPS, GameState &gameState);
  void resetAnimation() { animTimer = 0.f; }
  bool loadTextures();

  int getLives() const { return lives; }
  Direction getDirection() const { return direction; }
  sf::Sprite &getSprite() { return sprite; }
  sf::Vector2f getPosition() const { return position; }

  void setPosition(sf::Vector2f newPosition) { position = newPosition; }
  void setDirection(Direction newDir) { direction = newDir; }
  void loseLife();
  void resetLives() { lives = 3; }

private:
  static constexpr float SPEED = 3.0f;
  static constexpr int FRAME_SIZE = 32;
  static constexpr int FRAMES_PER_DIR = 2;
  static constexpr float ANIMATION_SPEED = 8.f;
  static constexpr int TEXTURE_COUNT = 4;

  Direction direction;
  sf::Sprite sprite;
  sf::Texture texture[TEXTURE_COUNT];
  const std::string texturePaths[TEXTURE_COUNT] = {
      "assets/textures/pacman_down.png", "assets/textures/pacman_left.png",
      "assets/textures/pacman_right.png", "assets/textures/pacman_up.png"};
  sf::Vector2f position;
  static inline int lives = 3;
  float animTimer = 0.f;
};

}

#endif
