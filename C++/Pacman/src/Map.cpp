#include "../include/Map.hpp"
namespace pacman {
const std::vector<std::string> Map::DEFAULT_MAP = {
    "############################", "#............##............#",
    "#.####.#####.##.#####.####.#", "#O####.#####.##.#####.####O#",
    "#..........................#", "#.####.##.########.##.####.#",
    "#......##....##....##......#", "######.#####.##.#####.######",
    "######.#####.##.#####.######", "######.##..........##.######",
    "######.##.########.##.######", "######.##.########.##.######",
    "#...........GGGG...........#", "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#", "#......##....##....##......#",
    "######.##.########.##.######", "######.##.########.##.######",
    "######.##..........##.######", "######.##.########.##.######",
    "######.##.########.##.######", "#............##............#",
    "#.####.#####.##.#####.####.#", "#.####.#####.##.#####.####.#",
    "#O..##................##..O#", "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###", "#......##....##P...##......#",
    "#.########################.#", "#..........................#",
    "############################"};
Map::Map() : map(DEFAULT_MAP) {
  pellet.setRadius(1);
  powerPellet.setRadius(5);
  pellet.setOutlineThickness(2.f);
  powerPellet.setOutlineThickness(3.f);
  pellet.setOutlineColor(sf::Color::Yellow);
  powerPellet.setOutlineColor(sf::Color::Yellow);
  wall.setFillColor(sf::Color::Blue);
}

Map::~Map() {}

void Map::resetMap() { map = DEFAULT_MAP; }

void Map::render(sf::RenderWindow &window) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      switch (map[y][x]) {
      case '#':
        wall.setSize({CELL_SIZE, CELL_SIZE});
        wall.setPosition({static_cast<float>(x * CELL_SIZE),
                          static_cast<float>(y * CELL_SIZE)});
        window.draw(wall);
        break;
      case '.':
        pellet.setPosition({static_cast<float>(x * CELL_SIZE + pelletOffset),
                            static_cast<float>(y * CELL_SIZE + pelletOffset)});
        window.draw(pellet);
        break;
      case 'O':
        powerPellet.setPosition(
            {static_cast<float>(x * CELL_SIZE + powerPelletOffset),
             static_cast<float>(y * CELL_SIZE + powerPelletOffset)});
        window.draw(powerPellet);
        break;
      case 'G':
      default:
        break;
      }
    }
  }
}

const sf::Vector2f Map::getPacmanStartPos() {
  sf::Vector2f offset(5.f, 5.f);
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x] == 'P') {
        return {x * CELL_SIZE + offset.x, y * CELL_SIZE + offset.y};
      }
    }
  }
  return {0.f, 0.f};
}

std::vector<sf::Vector2f> Map::getGhostStartPositions() {
  sf::Vector2f offset(5.f, 5.f);
  std::vector<sf::Vector2f> positions;
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x] == 'G') {
        positions.push_back(
            {x * CELL_SIZE + offset.x, y * CELL_SIZE + offset.y});
      }
    }
  }
  return positions;
}

bool Map::checkWallCollision(sf::Sprite &shape) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x] == '#') {
        sf::FloatRect wallBounds(
            sf::Vector2f(static_cast<float>(x * CELL_SIZE),
                         static_cast<float>(y * CELL_SIZE)),
            sf::Vector2f(static_cast<float>(CELL_SIZE),
                         static_cast<float>(CELL_SIZE)));
        if (shape.getGlobalBounds().findIntersection(wallBounds)) {
          return true;
        }
      }
    }
  }
  return false;
}

int Map::handlePelletCollision(sf::Sprite &pacmanShape) {
  sf::FloatRect bounds = pacmanShape.getGlobalBounds();
  sf::Vector2f center(bounds.position.x + bounds.size.x / 2.f,
                      bounds.position.y + bounds.size.y / 2.f);
  int col = static_cast<int>(center.x) / CELL_SIZE;
  int row = static_cast<int>(center.y) / CELL_SIZE;

  if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) {
    return 0;
  }

  char &tile = map[row][col];
  if (tile == '.') {
    tile = ' ';
    return 1;
  } else if (tile == 'O') {
    tile = ' ';
    return 2;
  }
  return 0;
}

bool Map::allPelletsEaten() const {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x] == '.' || map[y][x] == 'O') {
        return false;
      }
    }
  }
  return true;
}

sf::Vector2i Map::worldToTile(const sf::Vector2f &worldPosition) const {
  return {static_cast<int>(worldPosition.x) / static_cast<int>(CELL_SIZE),
          static_cast<int>(worldPosition.y) / static_cast<int>(CELL_SIZE)};
}

sf::Vector2f Map::tileToWorldCenter(const sf::Vector2i &tile) const {
  return {tile.x * CELL_SIZE + CELL_SIZE / 2.f,
          tile.y * CELL_SIZE + CELL_SIZE / 2.f};
}

bool Map::isWalkableTile(const sf::Vector2i &tile) const {
  if (tile.y < 0 || tile.y >= static_cast<int>(MAP_HEIGHT) || tile.x < 0 ||
      tile.x >= static_cast<int>(MAP_WIDTH)) {
    return false;
  }
  return map[tile.y][tile.x] != '#';
}

}
