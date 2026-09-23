#include <iostream>

#include "./Space.hpp"

namespace {
constexpr int BOUNDARY_OFFSET = 50;
constexpr int MOVE_SPEED = 10;

bool isTextureValid(const Texture2D &texture) { return texture.id != 0; }
} // namespace

SpaceShip::SpaceShip() {
  image = LoadTexture("img/spaceship.png");
  if (!isTextureValid(image)) {
    std::cerr << "Warning: Failed to load spaceship texture, using fallback"
              << std::endl;
    Image fallback = GenImageColor(1, 1, WHITE);
    image = LoadTextureFromImage(fallback);
    UnloadImage(fallback);
  }

  position.x = static_cast<float>(GetScreenWidth()) / 2.0f;
  position.y = static_cast<float>(GetScreenHeight()) - 25.0f;
}

SpaceShip::~SpaceShip() {
  if (isTextureValid(image)) {
    UnloadTexture(image);
  }
}

void SpaceShip::Draw() {
  DrawTexture(image, static_cast<int>(position.x), static_cast<int>(position.y),
              WHITE);
  if (laser.Active) {
    laser.Draw_Laser();
  }
}

void SpaceShip::IsCollision() {
  const int screenWidth = GetScreenWidth();
  const float rightBound = screenWidth - BOUNDARY_OFFSET;

  if (position.x > rightBound) {
    position.x = rightBound;
  } else if (position.x < 0) {
    position.x = 0;
  }
}

void SpaceShip::Move() {
  if (Lives <= 0) {
    return;
  }

  if (IsKeyDown(KEY_LEFT)) {
    position.x -= MOVE_SPEED;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    position.x += MOVE_SPEED;
  }
}

void SpaceShip::Shot() {
  if (laser.Active) {
    return;
  }

  Vector2 laserPos = {
      position.x + static_cast<float>(image.width) / 2.0f - 1.5f, position.y};
  laser.Init_Laser(laserPos);
  laser.Active = true;
}

void SpaceShip::Update_Laser() {
  if (laser.Active) {
    laser.Update_Laser();
  }
}
