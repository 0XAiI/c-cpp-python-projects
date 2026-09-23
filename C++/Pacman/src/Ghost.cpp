#include <SFML/System/Clock.hpp>
#include <cmath>

#include <algorithm>
#include <limits>
#include <random>
#include <stdexcept>

#include "../include/Ghost.hpp"

#include "../include/Map.hpp"

namespace pacman {

Ghost::Ghost()
    : sprite{sf::Sprite(texture[0]), sf::Sprite(texture[1]),
             sf::Sprite(texture[2]), sf::Sprite(texture[3])},
      randomEngine(std::random_device{}()) {
  for (int i = 0; i < MAX_GHOSTS; i++) {
    direction[i] = Direction::NONE;
  }
}

Ghost::~Ghost() {}

void Ghost::loadTextures() {
  for (int i = 0; i < MAX_GHOSTS; i++) {
    if (!texture[i].loadFromFile(texturePaths[i])) {
      throw std::runtime_error("Error loading ghost texture");
    }
    sprite[i].setTexture(texture[i]);
    sprite[i].setTextureRect(
        sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
    sprite[i].setPosition(position[i]);
    direction[i] = Direction::NONE;
    lastDecisionTile[i] = sf::Vector2i(-1, -1);
  }
}

void Ghost::resetPositions(const std::vector<sf::Vector2f> &spawnPositions) {
  for (int i = 0; i < MAX_GHOSTS && i < static_cast<int>(spawnPositions.size());
       i++) {
    position[i] = spawnPositions[i];
    sprite[i].setPosition(position[i]);
    direction[i] = Direction::NONE;
    lastDecisionTile[i] = sf::Vector2i(-1, -1);
  }
  mode = GhostMode::SCATTER;
  modeTimer = 0.f;
  frightenedTimer = 0.f;
  chasePhase = false;
}

void Ghost::resetState() {
  mode = GhostMode::SCATTER;
  modeTimer = 0.f;
  frightenedTimer = 0.f;
  animTimer = 0.f;
  chasePhase = false;
  for (int i = 0; i < MAX_GHOSTS; i++) {
    lastDecisionTile[i] = sf::Vector2i(-1, -1);
  }
  randomEngine = std::mt19937(std::random_device{}());
}

void Ghost::frighten(float durationSeconds) {
  frightenedTimer = durationSeconds;
  mode = GhostMode::FRIGHTENED;
}

void Ghost::render(sf::RenderWindow &window) {
  static sf::Clock animClock;
  float dt = animClock.restart().asSeconds();
  animTimer += dt * ANIMATION_SPEED;

  bool isFrightened = (mode == GhostMode::FRIGHTENED);
  float flashTimer = frightenedTimer;

  for (int i = 0; i < MAX_GHOSTS; i++) {
    sprite[i].setPosition(position[i]);

    if (isFrightened) {
      int frame;
      if (flashTimer <= 2.f && static_cast<int>(animTimer * 4) % 2 == 0) {
        frame = 2 + (static_cast<int>(animTimer) % 2);
      } else {
        frame = static_cast<int>(animTimer) % 2;
      }
      sprite[i].setTextureRect(
          sf::IntRect(sf::Vector2i(frame * FRAME_SIZE, 0),
                      sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
    } else {
      sprite[i].setTexture(texture[i]);
      int dirIndex = static_cast<int>(direction[i]);
      if (dirIndex >= 0 && dirIndex <= 3 && direction[i] != Direction::NONE) {
        int animFrame = static_cast<int>(animTimer) % 2;
        int frameX = (dirIndex * 2 + animFrame) * FRAME_SIZE;
        sprite[i].setTextureRect(sf::IntRect(
            sf::Vector2i(frameX, 0), sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
      } else {
        sprite[i].setTextureRect(sf::IntRect(
            sf::Vector2i(0, 0), sf::Vector2i(FRAME_SIZE, FRAME_SIZE)));
      }
    }
    window.draw(sprite[i]);
  }
}

bool Ghost::checkPacmanCollision(sf::Sprite &pacmanSprite) {
  for (int i = 0; i < MAX_GHOSTS; i++) {
    if (pacmanSprite.getGlobalBounds().findIntersection(
            sprite[i].getGlobalBounds())) {
      return true;
    }
  }
  return false;
}

void Ghost::update(const Map &map, sf::Vector2f pacmanPosition,
                   Direction pacmanDirection, float deltaTimer, const int FPS,
                   GameState gameState) {
  if (gameState == GameState::MENU || gameState == GameState::VICTORY ||
      gameState == GameState::LOSE)
    return;

  updateMode(deltaTimer);
  float movementStep = SPEED * FPS * deltaTimer;

  for (int i = 0; i < MAX_GHOSTS; i++) {
    if (isNearTileCenter(map, position[i])) {
      sf::Vector2i tile = map.worldToTile(
          position[i] + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f));
      if (tile != lastDecisionTile[i] || direction[i] == Direction::NONE) {
        position[i] = tileCenterToGhostPosition(map, tile);
        direction[i] = chooseDirection(i, map, pacmanPosition, pacmanDirection);
        lastDecisionTile[i] = tile;
      }
    }

    switch (direction[i]) {
    case Direction::UP:
      position[i].y -= movementStep;
      break;
    case Direction::DOWN:
      position[i].y += movementStep;
      break;
    case Direction::LEFT:
      position[i].x -= movementStep;
      break;
    case Direction::RIGHT:
      position[i].x += movementStep;
      break;
    default:
      break;
    }
    sprite[i].setPosition(position[i]);
  }
}

void Ghost::updateMode(float deltaTimer) {
  if (frightenedTimer > 0.f) {
    frightenedTimer -= deltaTimer;
    if (frightenedTimer <= 0.f) {
      frightenedTimer = 0.f;
      mode = chasePhase ? GhostMode::CHASE : GhostMode::SCATTER;
    }
    return;
  }

  modeTimer += deltaTimer;
  float phaseDuration = chasePhase ? CHASE_DURATION : SCATTER_DURATION;
  if (modeTimer >= phaseDuration) {
    modeTimer = 0.f;
    chasePhase = !chasePhase;
    mode = chasePhase ? GhostMode::CHASE : GhostMode::SCATTER;
  }
}

Direction Ghost::chooseDirection(int index, const Map &map,
                                 sf::Vector2f pacmanPosition,
                                 Direction pacmanDirection) {
  sf::Vector2f ghostCenter =
      position[index] + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f);
  sf::Vector2i currentTile = map.worldToTile(ghostCenter);
  std::vector<Direction> validDirs =
      getValidDirections(map, currentTile, direction[index]);
  if (validDirs.empty()) {
    return Direction::NONE;
  }

  if (mode == GhostMode::FRIGHTENED) {
    std::uniform_int_distribution<int> distribution(
        0, static_cast<int>(validDirs.size() - 1));
    return validDirs[distribution(randomEngine)];
  }

  sf::Vector2i targetTile =
      getTargetTile(index, map, pacmanPosition, pacmanDirection);
  Direction bestDirection = validDirs[0];
  float bestDistance = std::numeric_limits<float>::max();
  for (Direction dir : validDirs) {
    sf::Vector2i nextTile = currentTile + directionToTileOffset(dir);
    float currentDistance = distanceSquared(nextTile, targetTile);
    if (currentDistance < bestDistance) {
      bestDistance = currentDistance;
      bestDirection = dir;
    }
  }
  return bestDirection;
}

std::vector<Direction>
Ghost::getValidDirections(const Map &map, sf::Vector2i tile,
                          Direction currentDirection) const {
  std::vector<Direction> validDirs;
  std::vector<Direction> candidates = {Direction::UP, Direction::LEFT,
                                       Direction::DOWN, Direction::RIGHT};

  for (Direction dir : candidates) {
    sf::Vector2i nextTile = tile + directionToTileOffset(dir);
    if (map.isWalkableTile(nextTile)) {
      validDirs.push_back(dir);
    }
  }

  if (validDirs.size() > 1 && currentDirection != Direction::NONE) {
    Direction opposite = oppositeDirection(currentDirection);
    validDirs.erase(std::remove(validDirs.begin(), validDirs.end(), opposite),
                    validDirs.end());
  }

  if (validDirs.empty() && currentDirection != Direction::NONE) {
    Direction opposite = oppositeDirection(currentDirection);
    sf::Vector2i nextTile = tile + directionToTileOffset(opposite);
    if (map.isWalkableTile(nextTile)) {
      validDirs.push_back(opposite);
    }
  }

  return validDirs;
}

sf::Vector2i Ghost::getTargetTile(int index, const Map &map,
                                  sf::Vector2f pacmanPosition,
                                  Direction pacmanDirection) const {
  sf::Vector2f pacmanCenter =
      pacmanPosition + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f);
  sf::Vector2i pacmanTile = map.worldToTile(pacmanCenter);

  if (mode == GhostMode::SCATTER) {
    return scatterTargets[index];
  }

  sf::Vector2i pacmanDirOffset = directionToTileOffset(pacmanDirection);
  sf::Vector2f blinkyCenter =
      position[0] + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f);
  sf::Vector2i blinkyTile = map.worldToTile(blinkyCenter);

  switch (index) {
  case 0:
    return pacmanTile;
  case 1:
    return pacmanTile +
           sf::Vector2i(pacmanDirOffset.x * 4, pacmanDirOffset.y * 4);
  case 2: {
    sf::Vector2i aheadTile =
        pacmanTile + sf::Vector2i(pacmanDirOffset.x * 2, pacmanDirOffset.y * 2);
    sf::Vector2i vectorFromBlinky = aheadTile - blinkyTile;
    return aheadTile + vectorFromBlinky;
  }
  case 3: {
    sf::Vector2f clydeCenter =
        position[index] + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f);
    sf::Vector2i clydeTile = map.worldToTile(clydeCenter);
    if (distanceSquared(clydeTile, pacmanTile) > 64.f) {
      return pacmanTile;
    }
    return scatterTargets[index];
  }
  default:
    return pacmanTile;
  }
}

sf::Vector2i Ghost::directionToTileOffset(Direction dir) const {
  switch (dir) {
  case Direction::UP:
    return sf::Vector2i(0, -1);
  case Direction::DOWN:
    return sf::Vector2i(0, 1);
  case Direction::LEFT:
    return sf::Vector2i(-1, 0);
  case Direction::RIGHT:
    return sf::Vector2i(1, 0);
  default:
    return sf::Vector2i(0, 0);
  }
}

Direction Ghost::oppositeDirection(Direction dir) const {
  switch (dir) {
  case Direction::UP:
    return Direction::DOWN;
  case Direction::DOWN:
    return Direction::UP;
  case Direction::LEFT:
    return Direction::RIGHT;
  case Direction::RIGHT:
    return Direction::LEFT;
  default:
    return Direction::NONE;
  }
}

bool Ghost::isNearTileCenter(const Map &map, sf::Vector2f ghostPosition) const {
  sf::Vector2f ghostCenter =
      ghostPosition + sf::Vector2f(FRAME_SIZE / 2.f, FRAME_SIZE / 2.f);
  sf::Vector2i tile = map.worldToTile(ghostCenter);
  sf::Vector2f tileCenter = map.tileToWorldCenter(tile);
  return std::abs(ghostCenter.x - tileCenter.x) <= TILE_CENTER_EPSILON &&
         std::abs(ghostCenter.y - tileCenter.y) <= TILE_CENTER_EPSILON;
}

sf::Vector2f Ghost::tileCenterToGhostPosition(const Map &map,
                                              const sf::Vector2i &tile) const {
  sf::Vector2f center = map.tileToWorldCenter(tile);
  return sf::Vector2f(center.x - FRAME_SIZE / 2.f, center.y - FRAME_SIZE / 2.f);
}

float Ghost::distanceSquared(const sf::Vector2i &a,
                             const sf::Vector2i &b) const {
  float dx = static_cast<float>(a.x - b.x);
  float dy = static_cast<float>(a.y - b.y);
  return dx * dx + dy * dy;
}

} // namespace pacman
