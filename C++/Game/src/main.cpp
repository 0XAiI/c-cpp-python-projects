
#include <cstdlib>
#include <raylib.h>

#include <iostream>
#include <time.h>

#include "common.hpp"

#include "./Game.hpp"

#define WIDTH 700
#define HEIGHT 650

namespace {
const float SHOOT_COOLDOWN = 0.3f;

Sound g_laserSound;

void PlayLaserSound() { PlaySound(g_laserSound); }

void DrawEdges() {
  Rectangle rec = {0, 0, WIDTH, HEIGHT};
  DrawRectangleLinesEx(rec, 5, WHITE);
}

bool LoadSounds() {
  g_laserSound = LoadSound("Sound/laser.ogg");
  if (!Valid_Sound(g_laserSound)) {
    std::cerr << "Failed to load laser.ogg sound" << std::endl;
    return false;
  }
  return true;
}

void Cleanup() {
  if (Valid_Sound(g_laserSound)) {
    UnloadSound(g_laserSound);
  }
  CloseAudioDevice();
  CloseWindow();
}
} // namespace

int main(void) {
  srand(time(NULL));

  SetTargetFPS(60);

  InitWindow(WIDTH, HEIGHT, "SHOOTING GAME");
  if (!IsWindowReady()) {
    std::cerr << "Failed to create window" << std::endl;
    return EXIT_FAILURE;
  }

  InitAudioDevice();
  if (!IsAudioDeviceReady()) {
    std::cerr << "Failed to initialize audio device" << std::endl;
    CloseWindow();
    return EXIT_FAILURE;
  }

  if (!LoadSounds()) {
    Cleanup();
    return EXIT_FAILURE;
  }

  Game *game = new (std::nothrow) Game();
  if (!game) {
    std::cerr << "Failed to create game instance" << std::endl;
    Cleanup();
    return EXIT_FAILURE;
  }

  const Color background = {29, 29, 27, 255};
  float shootTimer = 0.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(background);
    DrawEdges();

    if (!game->GameOver()) {
      shootTimer -= GetFrameTime();
      if (shootTimer <= 0.0f && IsKeyPressed(KEY_SPACE)) {
        game->Shoot();
        PlayLaserSound();
        shootTimer = SHOOT_COOLDOWN;
      }
      game->UpdateLaser();
      game->Movement();
      game->IsCollision();
    }
    game->Draw();
    EndDrawing();
  }

  delete game;
  Cleanup();
  return EXIT_SUCCESS;
}
