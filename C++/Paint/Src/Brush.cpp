#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include "../Include/Brush.hpp"

namespace {
constexpr int DEFAULT_CENTER_X = 395;
constexpr int DEFAULT_CENTER_Y = 345;
constexpr int DEFAULT_RADIUS = 8;
} // namespace

void Brush::Init_Brush() {
  brush_struct.Center_X = DEFAULT_CENTER_X;
  brush_struct.Center_Y = DEFAULT_CENTER_Y;
  brush_struct.Radius = DEFAULT_RADIUS;
  brush_struct.Is_Drawing = false;
}

void Brush::Draw_Brush(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, Brush_Color.r, Brush_Color.g, Brush_Color.b,
                         Brush_Color.a);
  int32_t x = brush_struct.Radius - 1;
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = tx - (brush_struct.Radius * 2);

  while (x >= y) {
    SDL_RenderDrawLine(renderer, brush_struct.Center_X - x,
                       brush_struct.Center_Y - y, brush_struct.Center_X + x,
                       brush_struct.Center_Y - y);
    SDL_RenderDrawLine(renderer, brush_struct.Center_X - x,
                       brush_struct.Center_Y + y, brush_struct.Center_X + x,
                       brush_struct.Center_Y + y);

    SDL_RenderDrawLine(renderer, brush_struct.Center_X - y,
                       brush_struct.Center_Y - x, brush_struct.Center_X + y,
                       brush_struct.Center_Y - x);
    SDL_RenderDrawLine(renderer, brush_struct.Center_X - y,
                       brush_struct.Center_Y + x, brush_struct.Center_X + y,
                       brush_struct.Center_Y + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }
    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - (brush_struct.Radius * 2));
    }
  }
}

void Brush::Update_Brush(Brush_Struct *bs) {
  if (!bs)
    return;
  SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
  bs->Center_X = mouse_position.x;
  bs->Center_Y = mouse_position.y;
}

bool Brush::Check_Mouse_Interaction(Mouse_Position pos, SDL_FRect rect) {
  return (pos.x >= rect.x && pos.x <= rect.x + rect.w && pos.y >= rect.y &&
          pos.y <= rect.y + rect.h);
}

void Brush::Increase_Brush_Size(SDL_Event &ev, Brush_Struct &bs) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP) {
      bs.Radius += 2;
      Update_Brush_Size(bs.Radius);
    }
    if (ev.key.keysym.sym == SDLK_DOWN) {
      bs.Radius -= 2;
      Update_Brush_Size(bs.Radius);
    }
  }
}
