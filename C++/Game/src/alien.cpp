#include <cstdlib>
#include <ctime>

#include <iostream>

#include "alien.hpp"

namespace {
constexpr int SCREEN_WIDTH = 700;
constexpr int START_X = 50;
constexpr float ALIEN_MOVE_SPEED = 50.0f;

bool isTextureValid(Texture2D texture) { return texture.id != 0; }
} // namespace

void Alien::Init_Alien() {
  alien_texture[0] = LoadTexture("img/alien1.png");
  alien_texture[1] = LoadTexture("img/alien2.png");
  alien_texture[2] = LoadTexture("img/alien3.png");

  bool texturesLoaded = isTextureValid(alien_texture[0]) &&
                        isTextureValid(alien_texture[1]) &&
                        isTextureValid(alien_texture[2]);

  if (!texturesLoaded) {
    std::cerr << "Warning: Some alien textures failed to load" << std::endl;
  }

  shootTimer1 = shootCooldown;
  shootTimer2 = shootCooldown;
  shootTimer3 = shootCooldown;

  const int alienWidth = alien_texture[0].width;
  const float totalSpace = SCREEN_WIDTH - (2 * START_X);
  const float spacing =
      (totalSpace - (ALIENS_PER_ROW * alienWidth)) / (ALIENS_PER_ROW - 1);

  for (int i = 0; i < ALIENS_PER_ROW; i++) {
    const float xPos = START_X + i * (alienWidth + spacing);
    alien1_positions[i] = Vector2{xPos, 50};
    alien1_active[i] = true;

    alien2_positions[i] = Vector2{xPos, 150};
    alien2_positions_bottom[i] = Vector2{xPos, 200};
    alien2_active[i] = true;
    alien2_active_bottom[i] = true;

    alien3_positions[i] = Vector2{xPos, 250};
    alien3_active[i] = true;
  }

  for (int i = ALIENS_PER_ROW; i < MAX_ALIENS; i++) {
    alien1_active[i] = false;
    alien2_active[i] = false;
    alien2_active_bottom[i] = false;
    alien3_active[i] = false;
  }

  for (int i = 0; i < MAX_ALIENS; i++) {
    alien_laser[i].Active = false;
  }

  Alien2_Move = false;
  Alien3_Move = false;
  Alien2_Delay = ALIEN2_DELAY;
  Alien3_Delay = ALIEN3_DELAY;
}

void Alien::Draw_Alien1() {
  for (int i = 0; i < ALIENS_PER_ROW; i++) {
    if (alien1_active[i]) {
      DrawTexture(alien_texture[0], static_cast<int>(alien1_positions[i].x),
                  static_cast<int>(alien1_positions[i].y), WHITE);
    }
  }
}

void Alien::Draw_Alien2() {
  if (Alien2_Delay > 0) {
    Alien2_Delay -= GetFrameTime();
    if (Alien2_Delay <= 0) {
      Alien2_Move = true;
    }
  }

  const float maxY = GetScreenHeight() / 2.0f + 100.0f;
  const float maxYBottom = maxY + 50.0f;

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien2_active[i]) {
      if (Alien2_Move && alien2_positions[i].y < maxY) {
        alien2_positions[i].y += GetFrameTime() * ALIEN_MOVE_SPEED;
      }
      DrawTexture(alien_texture[1], static_cast<int>(alien2_positions[i].x),
                  static_cast<int>(alien2_positions[i].y), WHITE);
    }

    if (alien2_active_bottom[i]) {
      if (Alien2_Move && alien2_positions_bottom[i].y < maxYBottom) {
        alien2_positions_bottom[i].y += GetFrameTime() * ALIEN_MOVE_SPEED;
      }
      DrawTexture(alien_texture[1],
                  static_cast<int>(alien2_positions_bottom[i].x),
                  static_cast<int>(alien2_positions_bottom[i].y), WHITE);
    }
  }
}

void Alien::Draw_Alien3() {
  if (Alien3_Delay > 0) {
    Alien3_Delay -= GetFrameTime();
    if (Alien3_Delay <= 0) {
      Alien3_Move = true;
    }
  }

  const float maxY = GetScreenHeight() / 2.0f;

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien3_active[i]) {
      if (Alien3_Move && alien3_positions[i].y < maxY) {
        alien3_positions[i].y += GetFrameTime() * ALIEN_MOVE_SPEED;
      }
      DrawTexture(alien_texture[2], static_cast<int>(alien3_positions[i].x),
                  static_cast<int>(alien3_positions[i].y), WHITE);
    }
  }
}

bool Alien::IsAlienShotes(Laser laser) {
  if (!laser.Active) {
    return false;
  }

  const Rectangle laserRect = {laser.position.x, laser.position.y, 3.0f, 15.0f};

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien1_active[i]) {
      const Rectangle alienRect = {alien1_positions[i].x, alien1_positions[i].y,
                                   static_cast<float>(alien_texture[0].width),
                                   static_cast<float>(alien_texture[0].height)};
      if (CheckCollisionRecs(laserRect, alienRect)) {
        alien1_active[i] = false;
        return true;
      }
    }
  }

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien2_active[i]) {
      const Rectangle alienRect = {alien2_positions[i].x, alien2_positions[i].y,
                                   static_cast<float>(alien_texture[1].width),
                                   static_cast<float>(alien_texture[1].height)};
      if (CheckCollisionRecs(laserRect, alienRect)) {
        alien2_active[i] = false;
        return true;
      }
    }
    if (alien2_active_bottom[i]) {
      const Rectangle alienRect = {alien2_positions_bottom[i].x,
                                   alien2_positions_bottom[i].y,
                                   static_cast<float>(alien_texture[1].width),
                                   static_cast<float>(alien_texture[1].height)};
      if (CheckCollisionRecs(laserRect, alienRect)) {
        alien2_active_bottom[i] = false;
        return true;
      }
    }
  }

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien3_active[i]) {
      const Rectangle alienRect = {alien3_positions[i].x, alien3_positions[i].y,
                                   static_cast<float>(alien_texture[2].width),
                                   static_cast<float>(alien_texture[2].height)};
      if (CheckCollisionRecs(laserRect, alienRect)) {
        alien3_active[i] = false;
        return true;
      }
    }
  }
  return false;
}

bool Alien::All_Alien_Destroyed() {
  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien1_active[i] || alien2_active[i] || alien2_active_bottom[i] ||
        alien3_active[i]) {
      return false;
    }
  }
  return true;
}

void Alien::Alien_Shoot() {
  static bool seeded = false;
  if (!seeded) {
    srand(static_cast<unsigned>(time(nullptr)));
    seeded = true;
  }

  int alienIndex = rand() % MAX_ALIENS;

  for (int offset = 0; offset < MAX_ALIENS; offset++) {
    int currentIndex = (alienIndex + offset) % MAX_ALIENS;
    Vector2 laserPos = {0, 0};

    if (alien1_active[currentIndex]) {
      laserPos =
          Vector2{alien1_positions[currentIndex].x +
                      static_cast<float>(alien_texture[0].width) / 2.0f - 1.5f,
                  alien1_positions[currentIndex].y +
                      static_cast<float>(alien_texture[0].height)};
    } else if (alien2_active[currentIndex]) {
      laserPos =
          Vector2{alien2_positions[currentIndex].x +
                      static_cast<float>(alien_texture[1].width) / 2.0f - 1.5f,
                  alien2_positions[currentIndex].y +
                      static_cast<float>(alien_texture[1].height)};
    } else if (alien2_active_bottom[currentIndex]) {
      laserPos =
          Vector2{alien2_positions_bottom[currentIndex].x +
                      static_cast<float>(alien_texture[1].width) / 2.0f - 1.5f,
                  alien2_positions_bottom[currentIndex].y +
                      static_cast<float>(alien_texture[1].height)};
    } else if (alien3_active[currentIndex]) {
      laserPos =
          Vector2{alien3_positions[currentIndex].x +
                      static_cast<float>(alien_texture[2].width) / 2.0f - 1.5f,
                  alien3_positions[currentIndex].y +
                      static_cast<float>(alien_texture[2].height)};
    } else {
      continue;
    }

    for (int j = 0; j < MAX_ALIENS; j++) {
      if (!alien_laser[j].Active) {
        alien_laser[j].Init_Laser(laserPos);
        return;
      }
    }
  }
}

void Alien::Init_Alien_Laser() {
  for (int i = 0; i < MAX_ALIENS; i++) {
    alien_laser[i].Active = false;
  }
}

void Alien::Update_Alien_Laser() {
  shootTimer1 -= GetFrameTime();
  shootTimer2 -= GetFrameTime();
  shootTimer3 -= GetFrameTime();

  if (shootTimer1 <= 0.0f) {
    for (int i = 0; i < ALIENS_PER_ROW; i++) {
      if (alien1_active[i] && (rand() % 5 == 0)) {
        Vector2 laserPos = {
            alien1_positions[i].x +
                static_cast<float>(alien_texture[0].width) / 2.0f - 1.5f,
            alien1_positions[i].y +
                static_cast<float>(alien_texture[0].height)};
        for (int j = 0; j < MAX_ALIENS; j++) {
          if (!alien_laser[j].Active) {
            alien_laser[j].Init_Laser(laserPos);
            break;
          }
        }
      }
    }
    shootTimer1 = shootCooldown;
  }

  if (shootTimer2 <= 0.0f) {
    for (int i = 0; i < MAX_ALIENS; i++) {
      if ((alien2_active[i] || alien2_active_bottom[i]) && (rand() % 5 == 0)) {
        Vector2 laserPos;
        if (alien2_active[i]) {
          laserPos = {alien2_positions[i].x +
                          static_cast<float>(alien_texture[1].width) / 2.0f -
                          1.5f,
                      alien2_positions[i].y +
                          static_cast<float>(alien_texture[1].height)};
        } else {
          laserPos = {alien2_positions_bottom[i].x +
                          static_cast<float>(alien_texture[1].width) / 2.0f -
                          1.5f,
                      alien2_positions_bottom[i].y +
                          static_cast<float>(alien_texture[1].height)};
        }
        for (int j = 0; j < MAX_ALIENS; j++) {
          if (!alien_laser[j].Active) {
            alien_laser[j].Init_Laser(laserPos);
            break;
          }
        }
      }
    }
    shootTimer2 = shootCooldown;
  }

  if (shootTimer3 <= 0.0f) {
    for (int i = 0; i < MAX_ALIENS; i++) {
      if (alien3_active[i] && (rand() % 5 == 0)) {
        Vector2 laserPos = {
            alien3_positions[i].x +
                static_cast<float>(alien_texture[2].width) / 2.0f - 1.5f,
            alien3_positions[i].y +
                static_cast<float>(alien_texture[2].height)};
        for (int j = 0; j < MAX_ALIENS; j++) {
          if (!alien_laser[j].Active) {
            alien_laser[j].Init_Laser(laserPos);
            break;
          }
        }
      }
    }
    shootTimer3 = shootCooldown;
  }

  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien_laser[i].Active) {
      alien_laser[i].position.y += 6.0f;
      if (alien_laser[i].position.y > GetScreenHeight()) {
        alien_laser[i].Active = false;
      }
    }
  }
}

void Alien::Draw_Alien_Laser() {
  for (int i = 0; i < MAX_ALIENS; i++) {
    if (alien_laser[i].Active) {
      alien_laser[i].Draw_Laser();
    }
  }
}

void Alien::Destroy_Alien() {
  for (int i = 0; i < 3; i++) {
    if (alien_texture[i].id != 0) {
      UnloadTexture(alien_texture[i]);
      alien_texture[i].id = 0;
    }
  }

  for (int i = 0; i < MAX_ALIENS; i++) {
    alien_laser[i].Active = false;
    alien_laser[i].position = {0};
    alien1_active[i] = false;
    alien2_active[i] = false;
    alien2_active_bottom[i] = false;
    alien3_active[i] = false;
  }

  Alien2_Move = false;
  Alien3_Move = false;
  Alien2_Delay = ALIEN2_DELAY;
  Alien3_Delay = ALIEN3_DELAY;
}
