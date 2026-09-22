#pragma once

#ifndef ALIEN_HPP
#define ALIEN_HPP

#include "./Space.hpp"
#include <raylib.h>

class Alien {
public:
  static constexpr int MAX_ALIENS = 14;
  static constexpr int ALIENS_PER_ROW = 8;
  static constexpr float DEFAULT_SHOOT_COOLDOWN = 0.5f;
  static constexpr float ALIEN2_DELAY = 6.0f;
  static constexpr float ALIEN3_DELAY = 9.0f;

  void Init_Alien();
  void Draw_Alien1();
  void Draw_Alien2();
  void Draw_Alien3();
  void Init_Alien_Laser();
  void Alien_Shoot();
  void Draw_Alien_Laser();
  void Update_Alien_Laser();
  bool IsAlienShotes(Laser laser);
  bool All_Alien_Destroyed();
  void Destroy_Alien();

  Texture2D alien_texture[3] = {0};
  Vector2 alien1_positions[MAX_ALIENS] = {0};
  Vector2 alien2_positions[MAX_ALIENS] = {0};
  Vector2 alien2_positions_bottom[MAX_ALIENS] = {0};
  Vector2 alien3_positions[MAX_ALIENS] = {0};
  bool alien1_active[MAX_ALIENS] = {0};
  bool alien2_active[MAX_ALIENS] = {0};
  bool alien2_active_bottom[MAX_ALIENS] = {0};
  bool alien3_active[MAX_ALIENS] = {0};
  Laser alien_laser[MAX_ALIENS];

  float shootTimer1 = 0.0f;
  float shootTimer2 = 0.0f;
  float shootTimer3 = 0.0f;
  const float shootCooldown = DEFAULT_SHOOT_COOLDOWN;

  float Alien3_Delay = ALIEN3_DELAY;
  float Alien2_Delay = ALIEN2_DELAY;
  bool Alien2_Move = false;
  bool Alien3_Move = false;
};

#endif
