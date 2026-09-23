#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

#include <iostream>

#include "Player.hpp"

#include "Quiz.hpp"

Quiz quiz;

void Player::Init(void) {
  Pos = Vector2{355, 690};
  Texture = LoadTexture("Img/heart.png");
  if (Texture.id == 0) {
    std::cout << "Texture not loaded" << std::endl;
    return;
  }
}

void Player::ResetPos(Vector2 *Reset_Position) {
  *Reset_Position = Vector2{355, 690};
}

void Player::Draw(void) {
  DrawTextureEx(Texture, Pos, 0.0f, 0.1f, WHITE);
  quiz.Draw_Blocks(Pos);
  quiz.Draw_Choices();
  quiz.Draw_Question();
  quiz.SetChoices();
  quiz.Update_Choices();
}

void Player::Movement(void) {
  if (IsKeyDown(KEY_RIGHT)) {
    Pos.x += 5;
  }
  if (IsKeyDown(KEY_LEFT)) {
    Pos.x -= 5;
  }
  if (IsKeyDown(KEY_UP)) {
    Pos.y -= 5;
  }
  if (IsKeyDown(KEY_DOWN)) {
    Pos.y += 5;
  }
}

void Ranking(size_t Score) {
#if defined(_WIN32) || defined(_WIN64)
  system("cls");
#else
  system("clear");
#endif
  if (Score >= 8) {
    std::cout << "Excellent! You scored " << Score
              << "/10. Outstanding performance!" << std::endl;
  } else if (Score >= 5) {
    std::cout << "Good job! You scored " << Score << "/10. Keep it up!"
              << std::endl;
  } else {
    std::cout << "You scored " << Score << "/10. Better luck next time!"
              << std::endl;
  }
}

void Window(void) {
  Player player;
  InitAudioDevice();
  InitWindow(WIDTH, HEIGHT, TITLE);

  const Music music = LoadMusicStream("Music/music.mp3");
  const Font font = LoadFont("Font/Font2.ttf");
  const Vector2 ScorePos = {570, 20};
  SetMusicVolume(music, 0.5f);

  Quiz::LoadGameFont();
  Play_Music(music);
  SetTargetFPS(60);
  size_t Index = 0;
  player.Init();

  while (!WindowShouldClose()) {
    BeginDrawing();
    Timer();
    ClearBackground(BLACK);
    player.Movement();
    Draw_Cage(player);
    player.Draw();
    if (quiz.GetQuizAnswered() && Index < 10) {
      player.ResetPos(&player.Pos);
      quiz.QuizAnswered = false;
      quiz.Collision = false;
      Index++;
    }
    DrawTextEx(font, TextFormat("Resets: %d/10", Index + 1), ScorePos, 35, 5,
               YELLOW);
    WaitTime(0.016f);
    if (Index >= 10 || Quiz::Time_Finished) {
      break;
    }
    UpdateMusicStream(music);
    EndDrawing();
  }

  if (music.stream.buffer != nullptr) {
    StopMusicStream(music);
  }

  Texture texture = player.Texture;
  Quiz::UnloadGameFont();
  UnloadFont(font);
  UnloadMusicStream(music);
  UnloadTexture(texture);
  CloseAudioDevice();
  CloseWindow();
  Ranking(quiz.Score);
}

void Draw_Cage(Player &player) {
  DrawRectangleLines(50, 550, 650, 200, WHITE);
  if (player.Pos.x >= 645)
    player.Pos.x = 635;
  else if (player.Pos.x <= 50)
    player.Pos.x = 60;
  if (player.Pos.y <= 550)
    player.Pos.y = 560;
  else if (player.Pos.y >= 700)
    player.Pos.y = 690;
}

void Play_Music(Music music) {
  if (music.stream.buffer == 0) {
    std::cout << "Music not loaded" << std::endl;
    return;
  }
  PlayMusicStream(music);
}
