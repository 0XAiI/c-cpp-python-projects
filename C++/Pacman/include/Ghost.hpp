#ifndef GHOST_HPP
#define GHOST_HPP

#include "Entity.hpp"
#include "GameState.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <random>
#include <vector>

constexpr int MAX_GHOSTS = 4;
enum class GhostMode { SCATTER, CHASE, FRIGHTENED };

namespace pacman {
class Map;

class Ghost {
public:
  Ghost();
  ~Ghost();

  void render(sf::RenderWindow &window);
  void loadTextures();
  void update(const Map &map, sf::Vector2f pacmanPosition,
              Direction pacmanDirection, float deltaTimer, const int FPS,
              GameState gameState);
  void frighten(float durationSeconds = 6.f);
  void resetPositions(const std::vector<sf::Vector2f> &spawnPositions);
  void resetState();

  sf::Vector2f getPosition(unsigned int index) const { return position[index]; }
  sf::Sprite &getSprite(int index) { return sprite[index]; }
  void setPosition(unsigned int index, sf::Vector2f newPosition) {
    position[index] = newPosition;
  }
  GhostMode getMode() const { return mode; }

  bool checkPacmanCollision(sf::Sprite &pacmanSprite);

private:
  void updateMode(float deltaTimer);
  Direction chooseDirection(int index, const Map &map,
                            sf::Vector2f pacmanPosition,
                            Direction pacmanDirection);
  std::vector<Direction> getValidDirections(const Map &map, sf::Vector2i tile,
                                            Direction currentDirection) const;
  sf::Vector2i getTargetTile(int index, const Map &map,
                             sf::Vector2f pacmanPosition,
                             Direction pacmanDirection) const;
  sf::Vector2i directionToTileOffset(Direction dir) const;
  Direction oppositeDirection(Direction dir) const;
  bool isNearTileCenter(const Map &map, sf::Vector2f ghostPosition) const;
  sf::Vector2f tileCenterToGhostPosition(const Map &map,
                                         const sf::Vector2i &tile) const;
  float distanceSquared(const sf::Vector2i &a, const sf::Vector2i &b) const;

  sf::Texture texture[MAX_GHOSTS];
  sf::Sprite sprite[MAX_GHOSTS];
  Direction direction[MAX_GHOSTS];
  std::string texturePaths[MAX_GHOSTS] = {"assets/textures/ghost_blue_ss.png",
                                          "assets/textures/ghost_orange_ss.png",
                                          "assets/textures/ghost_pink_ss.png",
                                          "assets/textures/ghost_red_ss.png"};
  sf::Vector2f position[MAX_GHOSTS] = {};
  sf::Vector2i lastDecisionTile[MAX_GHOSTS] = {};
  sf::Vector2i scatterTargets[MAX_GHOSTS] = {
      sf::Vector2i(26, 1), sf::Vector2i(1, 1), sf::Vector2i(26, 29),
      sf::Vector2i(1, 29)};
  GhostMode mode = GhostMode::SCATTER;
  float modeTimer = 0.f;
  float frightenedTimer = 0.f;
  float animTimer = 0.f;
  bool chasePhase = false;
  std::mt19937 randomEngine;
  static constexpr float SPEED = 2.3f;
  static constexpr int FRAME_SIZE = 24;
  static constexpr float TILE_CENTER_EPSILON = 3.0f;
  static constexpr float SCATTER_DURATION = 7.f;
  static constexpr float CHASE_DURATION = 20.f;
  static constexpr float ANIMATION_SPEED = 8.f;
};

}

#endif
