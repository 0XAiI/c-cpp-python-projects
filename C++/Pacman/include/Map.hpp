#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

namespace pacman {

class Map {
public:
  Map();
  ~Map();

  void render(sf::RenderWindow &window);

  const sf::Vector2f getPacmanStartPos();
  std::vector<sf::Vector2f> getGhostStartPositions();

  bool checkWallCollision(sf::Sprite &shape);
  int handlePelletCollision(sf::Sprite &pacmanShape);

  bool allPelletsEaten() const;
  void resetMap();

  sf::Vector2i worldToTile(const sf::Vector2f &worldPosition) const;
  sf::Vector2f tileToWorldCenter(const sf::Vector2i &tile) const;
  bool isWalkableTile(const sf::Vector2i &tile) const;
  constexpr int getCellSize() const { return CELL_SIZE; }

private:
  static constexpr int MAP_WIDTH = 28;
  static constexpr int MAP_HEIGHT = 31;
  static constexpr int CELL_SIZE = 32;

  sf::RectangleShape wall;
  sf::CircleShape pellet;
  sf::CircleShape powerPellet;

  const int pelletOffset = 15;
  const int powerPelletOffset = 10;

  static const std::vector<std::string> DEFAULT_MAP;
  std::vector<std::string> map;
};

}

#endif
