#pragma once

#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

#define WIDTH 800
#define HEIGHT 800
#define TITLE "Window"

class Player {
public:
  Vector2 Pos;
  Texture2D Texture;
  void Init();
  void Draw();
  void Movement();
  void ResetPos(Vector2 *Reset_Position);
  ~Player() {}
};

void Ranking(size_t Score);
void Window();
void Draw_Cage(Player &player);
void Play_Music(Music music);
