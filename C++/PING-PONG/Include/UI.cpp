#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <cmath>

#include <iostream>
#include <string>

#include "UI.hpp"

namespace {
constexpr int RECT_WIDTH = 18;
constexpr int RECT_HEIGHT_MULT = 10;
constexpr int BALL_RADIUS = 25;
constexpr float BALL_VELOCITY = 2.5f;
constexpr int AI_MOVE_SPEED = 10;
}

static inline bool Load_Icon(SDL_Surface *&icon, const std::string file_path) {
  icon = IMG_Load(file_path.c_str());
  if (icon == nullptr) {
    std::cout << "Failed to load icon image: " << IMG_GetError() << std::endl;
    return false;
  }
  return true;
}

UI::UI() {
  score.player_score1 = 0;
  score.player_score2 = 0;
  winner.player1 = false;
  winner.player2 = false;

  window = nullptr;
  renderer = nullptr;
  icon = nullptr;

  Rect1 = {0, 0, 0, 0};
  Rect2 = {0, 0, 0, 0};
  ball = {false, false, 0, 0, 0.0f, 0.0f, 0.0f};
}

UI::~UI() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  IMG_Quit();
  SDL_Quit();
}

bool UI::Init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("SDL could not initialize! SDL error: %s", SDL_GetError());
    return false;
  }

  window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!window) {
    SDL_Log("Window could not be created! SDL error: %s", SDL_GetError());
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_Log("Renderer could not be created! SDL error: %s", SDL_GetError());
    return false;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    SDL_Log("SDL_image could not initialize! Error: %s", IMG_GetError());
    return false;
  }

  return true;
}

void UI::Init_Rects(SDL_FRect *Rect1, SDL_FRect *Rect2) {
  Rect1->w = RECT_WIDTH;
  Rect1->h = static_cast<float>(RECT_WIDTH * RECT_HEIGHT_MULT);
  Rect1->x = static_cast<float>(SCREEN_WIDTH) - Rect1->w - 10.0f;
  Rect1->y = static_cast<float>(SCREEN_HEIGHT) / 2.0f - 85.0f;

  Rect2->w = RECT_WIDTH;
  Rect2->h = static_cast<float>(RECT_WIDTH * RECT_HEIGHT_MULT);
  Rect2->x = 10.0f;
  Rect2->y = static_cast<float>(SCREEN_HEIGHT) / 2.0f - 85.0f;
}

void UI::Init_Ball(struct Ball *ball) {
  ball->x = static_cast<float>(SCREEN_WIDTH) / 2.0f;
  ball->y = static_cast<float>(SCREEN_HEIGHT) / 2.0f - 20.0f;
  ball->velX = BALL_VELOCITY;
  ball->velY = BALL_VELOCITY;
  ball->radius = BALL_RADIUS;
  ball->IsMoving = false;
  ball->BallHitWall = false;
}

void UI::Draw_Rects(SDL_Renderer *renderer, SDL_FRect &Rect1,
                    SDL_FRect &Rect2) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRectF(renderer, &Rect1);
  SDL_RenderFillRectF(renderer, &Rect2);

  SDL_RenderDrawRectF(renderer, &Rect1);
  SDL_RenderDrawRectF(renderer, &Rect2);

  const int centerX = SCREEN_WIDTH / 2;
  for (int i = -3; i < 4; i++) {
    SDL_RenderDrawLine(renderer, centerX + i, 0, centerX + i, SCREEN_HEIGHT);
  }
}

void UI::Draw_Ball(SDL_Renderer *renderer, struct Ball ball) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

  const int maxRadius = static_cast<int>(ball.radius);
  for (int r = maxRadius; r > 0; r--) {
    for (int angle = 0; angle < 360; angle++) {
      const float rad = angle * M_PI / 180.0f;
      const int x = static_cast<int>(ball.x + r * std::cos(rad));
      const int y = static_cast<int>(ball.y + r * std::sin(rad));
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
}

void UI::Update_Rect(SDL_FRect &Rect, SDL_Event &event) {
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP:
      Rect.y -= 15.0f;
      break;
    case SDLK_DOWN:
      Rect.y += 15.0f;
      break;
    default:
      break;
    }
  }
}

void UI::Handle_AI_Movement(SDL_FRect &Rect2, struct Ball &ball) {
  if (Rect2.y > ball.y + ball.radius) {
    Rect2.y -= AI_MOVE_SPEED;
  } else if (Rect2.y < ball.y - ball.radius) {
    Rect2.y += AI_MOVE_SPEED;
  }
}

void UI::Update_Ball(struct Ball *ball) {
  if (!ball->IsMoving) {
    return;
  }

  ball->x += ball->velX;
  ball->y += ball->velY;

  if (ball->y - ball->radius < 0) {
    ball->y = ball->radius;
    ball->velY = -ball->velY;
    ball->BallHitWall = true;
  } else if (ball->y + ball->radius > SCREEN_HEIGHT) {
    ball->y = static_cast<float>(SCREEN_HEIGHT) - ball->radius;
    ball->velY = -ball->velY;
    ball->BallHitWall = true;
  } else {
    ball->BallHitWall = false;
  }
}

void UI::Set_Window() {
  if (!Init()) {
    std::cerr << "Failed to initialize window and renderer: " << SDL_GetError()
              << std::endl;
    return;
  }

  Init_Ball(&ball);
  Init_Rects(&Rect1, &Rect2);
  const std::string file_path = "../Img/PIPO.png";
  if (Load_Icon(icon, file_path)) {
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
  }
}
