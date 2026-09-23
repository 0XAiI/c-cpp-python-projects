#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "../include/Pacman.hpp"

#include "../include/Entity.hpp"

namespace pacman {

Pacman::Pacman() : sprite(sf::Sprite(texture[0])) {
  direction = Direction::RIGHT;
}

Pacman::~Pacman() {}

bool Pacman::loadTextures() {
  for (int i = 0; i < TEXTURE_COUNT; i++) {
    if (!texture[i].loadFromFile(texturePaths[i]))
      throw std::runtime_error("Failed to load " + texturePaths[i]);
  }
  sprite.setScale(sf::Vector2f(0.75f, 0.75f));
  sprite.setTextureRect(
      sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
  sprite.setTexture(texture[2]);
  return true;
}

void Pacman::render(sf::RenderWindow &window) {
  static sf::Clock animClock;
  float dt = animClock.restart().asSeconds();

  sprite.setPosition(position);

  static int textureIndex;
  switch (direction) {
  case Direction::DOWN:
    textureIndex = 0;
    break;
  case Direction::LEFT:
    textureIndex = 1;
    break;
  case Direction::RIGHT:
    textureIndex = 2;
    break;
  case Direction::UP:
    textureIndex = 3;
    break;
  case Direction::NONE:
    break;
  default:
    break;
  }

  sprite.setTexture(texture[textureIndex]);

  if (direction != Direction::NONE) {
    animTimer += dt * ANIMATION_SPEED;
    int frame = static_cast<int>(animTimer) % FRAMES_PER_DIR;
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(frame * FRAME_SIZE, 0),
                                      sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
  } else {
    animTimer = 0.f;
    sprite.setTextureRect(
        sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
  }

  window.draw(sprite);
}

void Pacman::update(float deltaTimer, const int FPS, GameState &gameState) {
  if (gameState != GameState::PLAYING)
    return;

  float step = SPEED * FPS * deltaTimer;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) {
    position.y -= step;
    direction = Direction::UP;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) {
    position.y += step;
    direction = Direction::DOWN;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
    position.x -= step;
    direction = Direction::LEFT;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
    position.x += step;
    direction = Direction::RIGHT;
  } else {
    direction = Direction::NONE;
  }
  sprite.setPosition(position);
}

void Pacman::loseLife() {
  lives--;
  if (lives <= 0) {
    lives = 0;
  }
}

} // namespace pacman
