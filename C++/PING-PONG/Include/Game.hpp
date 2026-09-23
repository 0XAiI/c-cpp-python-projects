
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <memory>

#include "UI.hpp"

class Game {
  friend class Game_Data;

public:
  bool Rect_Collision_Ball(SDL_FRect *rect, struct Ball *ball);

  bool Check_Rec_Collsion(SDL_FRect &Rect);

  bool Check_Ball_Collision(struct Ball &ball);

  bool Check_Winner() {
    return ui->score.player_score1 > ui->score.player_score2;
  }

  unsigned int GetPlayer1Score() const { return ui->score.player_score1; }
  unsigned int GetPlayer2Score() const { return ui->score.player_score2; }

  void Get_Score(Winner winner);

  void Game_Update(SDL_FRect Current_Pos[2]);

  void Show_Scores(unsigned int *score1, unsigned int *score2);

  void Game_Reset(SDL_FRect *Rect1, SDL_FRect *Rect2);

  void Handle_Events(SDL_Event *event, bool &IsRunning);

  void Handle_Collision(SDL_FRect &Rect, SDL_FRect Current_Pos);

  void Main_Loop();

private:
  SDL_Color bgColor = {0, 0, 0, 255};
  std::unique_ptr<UI> ui = std::make_unique<UI>();
};
