#include <cstdio>

#include <iostream>

#include "./Game.hpp"

#include "Space.hpp"

#include "./alien.hpp"

namespace {
Sound g_explosionSound;

bool isSoundValid(Sound sound) { return sound.frameCount > 0; }
}

Game::Game() {
  loadFont();
  alien.Init_Alien();
  alien.Init_Alien_Laser();

  g_explosionSound = LoadSound("Sound/explosion.ogg");
  if (!isSoundValid(g_explosionSound)) {
    std::cerr << "Warning: Failed to load explosion.ogg" << std::endl;
  }
}

Game::~Game() {
  Destroy();
  if (isSoundValid(g_explosionSound)) {
    UnloadSound(g_explosionSound);
  }
}

void Game::loadFont() {
  font = LoadFont("Font/Score_Font.ttf");
  if (!ValidFont(font)) {
    std::cerr << "Warning: Failed to load font, using default" << std::endl;
  }
}

bool Game::GameOver() {
  return alien.All_Alien_Destroyed() || spaceShip.Lives <= 0;
}

void Game::IsCollision() {
  spaceShip.IsCollision();

  if (alien.IsAlienShotes(spaceShip.laser)) {
    spaceShip.laser.Active = false;
    if (isSoundValid(g_explosionSound)) {
      PlaySound(g_explosionSound);
    }
    Score += 10;
  }

  if (IsSpaceShipHitByAlienLaser()) {
    if (isSoundValid(g_explosionSound)) {
      PlaySound(g_explosionSound);
    }
    spaceShip.Lives--;
    spaceShip.laser.Active = false;
  }
}

bool Game::IsSpaceShipHitByAlienLaser() {
  const Rectangle shipRect = {spaceShip.position.x, spaceShip.position.y,
                              static_cast<float>(spaceShip.image.width),
                              static_cast<float>(spaceShip.image.height)};

  for (int i = 0; i < 14; i++) {
    if (!alien.alien_laser[i].Active)
      continue;

    const Rectangle laserRect = {alien.alien_laser[i].position.x,
                                 alien.alien_laser[i].position.y, 3.0f, 15.0f};

    if (CheckCollisionRecs(shipRect, laserRect)) {
      alien.alien_laser[i].Active = false;
      return true;
    }
  }
  return false;
}

void Game::Shoot() {
  spaceShip.Shot();
  alien.Alien_Shoot();
}

void Game::Movement() { spaceShip.Move(); }

void Game::Draw() {
  spaceShip.Draw();
  alien.Draw_Alien_Laser();

  alien.Draw_Alien1();
  alien.Draw_Alien2();
  alien.Draw_Alien3();

  if (GameOver()) {
    DrawText("GAME OVER", 200, 200, 40, RED);
    DrawText(TextFormat("Score : %i", Best_Score()), 200, 250, 40, RED);
  } else {
    const Vector2 livesPos = {static_cast<float>(GetScreenWidth()) - 150.0f,
                              15.0f};
    DrawTextEx(font, TextFormat("Lives : %i", spaceShip.Lives), livesPos, 25, 5,
               RED);

    if (ValidFont(font)) {
      const Vector2 scorePos = {
          static_cast<float>(GetScreenWidth()) / 2.0f - 75.0f, 15.0f};
      DrawTextEx(font, TextFormat("Score : %i", Game_Score()), scorePos, 25, 5,
                 RED);
    } else {
      DrawText(TextFormat("Score : %i", Game_Score()),
               GetScreenWidth() / 2 - 75, 15, 25, RED);
    }
  }
}

void Game::Destroy() {
  if (ValidFont(font)) {
    UnloadFont(font);
  }
  alien.Destroy_Alien();
}

void Game::UpdateLaser() {
  spaceShip.Update_Laser();
  alien.Update_Alien_Laser();
}

int Game::Game_Score() { return Score; }

int Game::Best_Score() {
  int bestScore = 0;
  FILE *file = fopen("Score.txt", "r");
  if (file) {
    if (fscanf(file, "%d", &bestScore) != 1) {
      bestScore = 0;
    }
    fclose(file);
  }

  if (Score > bestScore) {
    bestScore = Score;
    FILE *writeFile = fopen("Score.txt", "w");
    if (writeFile) {
      fprintf(writeFile, "%d", bestScore);
      fclose(writeFile);
    }
  }
  return bestScore;
}
