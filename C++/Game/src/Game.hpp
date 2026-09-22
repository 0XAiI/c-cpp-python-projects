
#ifndef GAME_HPP
#define GAME_HPP

#include "Space.hpp"
#include "alien.hpp"
#include <raylib.h>

class Game {
public:
  Game();
  ~Game();

  void Movement();
  void Draw();
  void IsCollision();
  void Destroy();
  int Game_Score();
  bool IsSpaceShipHitByAlienLaser();
  int Best_Score();
  void Shoot();
  bool GameOver();
  void UpdateLaser();

private:
  void loadFont();
  static bool ValidFont(const Font &font) { return font.texture.id != 0; }

  SpaceShip spaceShip;
  Alien alien;
  Font font = {0};
  int Score = 0;
};

#endif
