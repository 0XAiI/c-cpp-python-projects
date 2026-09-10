#include <SDL2/SDL_render.h>

extern "C" {
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
}

#include "Game.hpp"
#include "UI.hpp"
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

static ma_engine engine;
static bool audio_init = false;
static TTF_Font *gameFont = nullptr;

static inline bool Init_Sound() {
  if (ma_engine_init(nullptr, &engine) == MA_SUCCESS) {
    audio_init = true;
    return true;
  }
  return false;
}

static inline void Play_Sound() {
  if (audio_init) {
    ma_engine_play_sound(&engine, "../Sound/Ping-pong-ball.wav", NULL);
  }
}

static inline void Cleanup_Sound() {
  if (audio_init) {
    ma_engine_uninit(&engine);
  }
  if (gameFont) {
    TTF_CloseFont(gameFont);
  }
}

bool Game::Check_Rec_Collsion(SDL_FRect &Rect) {
  if (Rect.y < 0 || Rect.y + Rect.h > SCREEN_HEIGHT) {
    return true;
  }
  return false;
}

bool Game::Check_Ball_Collision(struct Ball &ball) {
  if (ball.x + ball.radius > SCREEN_WIDTH) {

    ui->winner.player1 = false;
    ui->winner.player2 = true;
    ui->score.player_score2 += 1;

    ball.x = (float)SCREEN_WIDTH / 2;
    ball.y = (float)SCREEN_HEIGHT / 2;
    ball.velX = -std::abs(ball.velX);
    ball.IsMoving = false;

    return true;
  } else if (ball.x - ball.radius < 0) {
    ui->winner.player1 = true;
    ui->winner.player2 = false;
    ui->score.player_score1 += 1;

    ball.x = (float)SCREEN_WIDTH / 2;
    ball.y = (float)SCREEN_HEIGHT / 2;
    ball.velX = std::abs(ball.velX);
    ball.IsMoving = false;

    return true;
  }

  return false;
}

bool Game::Rect_Collision_Ball(SDL_FRect *rect, struct Ball *ball) {

  float closestX = ball->x;
  if (ball->x < rect->x)
    closestX = rect->x;
  else if (ball->x > rect->x + rect->w)
    closestX = rect->x + rect->w;

  float closestY = ball->y;
  if (ball->y < rect->y)
    closestY = rect->y;
  else if (ball->y > rect->y + rect->h)
    closestY = rect->y + rect->h;

  float dx = ball->x - closestX;
  float dy = ball->y - closestY;

  return (dx * dx + dy * dy) <= (ball->radius * ball->radius);
}

void Game::Game_Reset(SDL_FRect *Rect1, SDL_FRect *Rect2) {
  ui->Init_Rects(Rect1, Rect2);
  ui->Init_Ball(&ui->ball);
  ui->ball.IsMoving = false;
}

void Game::Get_Score(Winner winner) {
  if (winner.player1) {
    ui->winner.player1 = false;
  } else if (winner.player2) {
    ui->winner.player2 = false;
  }
}

void Game::Handle_Events(SDL_Event *event, bool &IsRunning) {
  switch (event->type) {
  case SDL_QUIT:
    IsRunning = false;
    break;
  case SDL_KEYDOWN:
    if (event->key.keysym.sym == SDLK_ESCAPE)
      IsRunning = false;
    if (event->key.keysym.sym == SDLK_SPACE ||
        event->key.keysym.sym == SDLK_RETURN)
      ui->ball.IsMoving = true;
    if (event->key.keysym.sym == SDLK_q)
      IsRunning = false;
    break;
  default:
    break;
  }
  ui->Update_Rect(ui->Rect1, *event);
  Get_Score(ui->winner);
}

void Game::Handle_Collision(SDL_FRect &Rect, SDL_FRect Current_Pos) {
  if (Check_Rec_Collsion(Rect)) {
    Rect.y = Current_Pos.y;
  }
}

void Game::Show_Scores(unsigned int *score1, unsigned int *score2) {

  if (!gameFont) {
    gameFont = TTF_OpenFont("../Font/Font.ttf", 40);
    if (!gameFont)
      return;
  }

  SDL_Color white = {255, 255, 255, 255};

  // Create score texts
  const std::string scoreText1 = "P1: " + std::to_string(*score1);
  const std::string scoreText2 = "P2: " + std::to_string(*score2);

  // Render Player 1 score (right side)
  SDL_Surface *surface1 =
      TTF_RenderText_Blended(gameFont, scoreText1.c_str(), white);
  if (surface1) {
    SDL_Texture *texture1 =
        SDL_CreateTextureFromSurface(ui->renderer, surface1);
    if (texture1) {
      // Position Player 1 score on the right side
      SDL_FRect dstRect1 = {static_cast<float>(SCREEN_WIDTH - surface1->w - 50),
                            20.0f, static_cast<float>(surface1->w),
                            static_cast<float>(surface1->h)};
      SDL_RenderCopyF(ui->renderer, texture1, NULL, &dstRect1);
      SDL_DestroyTexture(texture1);
    }
    SDL_FreeSurface(surface1);
  }

  // Render Player 2 score (left side)
  SDL_Surface *surface2 =
      TTF_RenderText_Blended(gameFont, scoreText2.c_str(), white);
  if (surface2) {
    SDL_Texture *texture2 =
        SDL_CreateTextureFromSurface(ui->renderer, surface2);
    if (texture2) {
      // Position Player 2 score on the left side
      SDL_FRect dstRect2 = {50.0f, 20.0f, static_cast<float>(surface2->w),
                            static_cast<float>(surface2->h)};
      SDL_RenderCopyF(ui->renderer, texture2, NULL, &dstRect2);
      SDL_DestroyTexture(texture2);
    }
    SDL_FreeSurface(surface2);
  }
}

void Game::Game_Update(SDL_FRect Current_Pos[2]) {
  if (Check_Rec_Collsion(ui->Rect1))
    ui->Rect1 = Current_Pos[0];
  if (Check_Rec_Collsion(ui->Rect2))
    ui->Rect2 = Current_Pos[1];

  SDL_SetRenderDrawColor(ui->renderer, bgColor.r, bgColor.g, bgColor.b,
                         bgColor.a);
  SDL_RenderClear(ui->renderer);
  ui->Draw_Rects(ui->renderer, ui->Rect1, ui->Rect2);
  ui->Draw_Ball(ui->renderer, ui->ball);

  ui->Update_Ball(&ui->ball);

  if (ui->ball.BallHitWall) {
    Play_Sound();
  }

  ui->Handle_AI_Movement(ui->Rect2, ui->ball);
  Handle_Collision(ui->Rect1, Current_Pos[0]);
  Handle_Collision(ui->Rect2, Current_Pos[1]);

  if (Rect_Collision_Ball(&ui->Rect1, &ui->ball)) {
    ui->ball.velX =
        -std::abs(ui->ball.velX); // Move left after hitting right paddle
    Play_Sound();
  }
  if (Rect_Collision_Ball(&ui->Rect2, &ui->ball)) {
    ui->ball.velX =
        std::abs(ui->ball.velX); // Move right after hitting left paddle
    Play_Sound();
  }
  if (ui->winner.player1 || ui->winner.player2) {
    Game_Reset(&ui->Rect1, &ui->Rect2);
  }

  Check_Ball_Collision(ui->ball);

  Show_Scores(&ui->score.player_score1, &ui->score.player_score2);

  SDL_RenderPresent(ui->renderer);
}

void Game::Main_Loop() {

  // Set Window Icon
  ui->Set_Window();

  // Initialize sound engine
  if (!Init_Sound()) {
    std::cerr << "Failed to initialize sound engine." << std::endl;
    return;
  }

  bool IsRunning = true;
  Uint64 startTime = SDL_GetPerformanceCounter();
  Uint64 frequency = SDL_GetPerformanceFrequency();
  while (IsRunning) {
    SDL_Event event;
    SDL_FRect Current_Pos[2] = {ui->Rect1, ui->Rect2};
    Uint64 currentTime = SDL_GetPerformanceCounter();
    float elapsedSeconds = (float)(currentTime - startTime) / frequency;
    while (SDL_PollEvent(&event)) {
      Handle_Events(&event, IsRunning);
    }
    if (elapsedSeconds >= 5.0f) {
      ui->ball.velX *= 1.001f;
      ui->ball.velY *= 1.001f;
    }
    Game_Update(Current_Pos);
  }
  Cleanup_Sound();
}
