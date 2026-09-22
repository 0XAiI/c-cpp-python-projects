#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <cstdint>

#include <iostream>

#include "../Include/UI.hpp"

namespace {
constexpr int NUM_COLORS = 9;
constexpr int FPS_DELAY = 16;
constexpr uint32_t BG_COLOR = 0x000000FF;
}

const std::string UI::Icon_Path = "../Icon/icon.png";

UI::UI() {
  window = nullptr;
  renderer = nullptr;
  texture = nullptr;
  event = new (std::nothrow) SDL_Event;
  if (event) {
    rects.resize(NUM_COLORS);
  }
  lastDrawX = -1;
  lastDrawY = -1;
  Change_Background = false;
}

UI::~UI() {
  if (texture) {
    SDL_DestroyTexture(texture);
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  if (event) {
    delete event;
  }
  SDL_Quit();
}

void UI::Load_Icon() {
  if (!window)
    return;

  SDL_Surface *icon = IMG_Load(Icon_Path.c_str());
  if (!icon) {
    std::cerr << "Error loading icon: " << SDL_GetError() << std::endl;
    return;
  }
  SDL_SetWindowIcon(window, icon);
  SDL_FreeSurface(icon);
}

void UI::Event_Handler(SDL_Event &ev, bool &running) {
  switch (ev.type) {
  case SDL_QUIT:
    running = false;
    break;

  case SDL_MOUSEBUTTONDOWN:
    if (ev.button.button == SDL_BUTTON_LEFT) {
      brush.Get_Brush_Struct()->Is_Drawing = true;
    }
    break;

  case SDL_MOUSEBUTTONUP:
    if (ev.button.button == SDL_BUTTON_LEFT) {
      brush.Get_Brush_Struct()->Is_Drawing = false;
      lastDrawX = -1;
      lastDrawY = -1;
    }
    break;

  case SDL_KEYDOWN:
    switch (ev.key.keysym.sym) {
    case SDLK_ESCAPE:
    case SDLK_q:
      running = false;
      break;
    case SDLK_s:
      save.Save_To_Png(renderer, texture);
      break;
    case SDLK_RETURN:
      Change_Background = true;
      break;
    default:
      break;
    }
    break;
  }
}

void UI::Init_Color_Box() {
  for (int i = 0; i < NUM_COLORS; i++) {
    rects[i] = {static_cast<float>(i) * 50.0f, 0.0f, 50.0f, 50.0f};
  }
}

void UI::Draw_Color_Box() {
  static const SDL_Color colors[NUM_COLORS] = {
      {0, 0, 0, 255},     {255, 255, 255, 255}, {255, 0, 0, 255},
      {0, 255, 0, 255},   {0, 0, 255, 255},     {255, 255, 0, 255},
      {255, 0, 255, 255}, {0, 255, 255, 255},   {255, 165, 0, 255},
  };

  for (int i = 0; i < NUM_COLORS; i++) {
    SDL_FRect &rect = rects[i];
    const SDL_Color &color = colors[i];

    if (brush.Check_Mouse_Interaction(brush.Get_Mouse_Position(), rects[i]) &&
        brush.Get_Brush_Struct()->Is_Drawing) {
      brush.Update_Brush_Color(const_cast<SDL_Color &>(color));
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRectF(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRectF(renderer, &rect);
  }
}

bool UI::Init_Window() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return false;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window Init Error: " << SDL_GetError() << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer Init Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    window = nullptr;
    return false;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH,
                              WINDOW_HEIGHT);
  if (!texture) {
    std::cerr << "Texture Init Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    return false;
  }

  SDL_SetWindowResizable(window, SDL_TRUE);
  Load_Icon();
  brush.Init_Brush();
  Init_Color_Box();
  return true;
}

void UI::Draw_To_Texture(int x, int y, int radius, SDL_Color color) {
  void *pixels;
  int pitch;
  if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
    std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
    return;
  }

  uint32_t *pixelData = (uint32_t *)pixels;
  uint32_t brushColor =
      (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;

  Draw_Circle_To_Pixels(pixelData, pitch, x, y, radius, brushColor);
  SDL_UnlockTexture(texture);
}

void UI::Draw_Circle_To_Pixels(uint32_t *pixelData, int pitch, int x, int y,
                               int radius, uint32_t brushColor) {
  int startX = std::max(0, x - radius);
  int endX = std::min(WINDOW_WIDTH - 1, x + radius);
  int startY = std::max(0, y - radius);
  int endY = std::min(WINDOW_HEIGHT - 1, y + radius);

  for (int px = startX; px <= endX; ++px) {
    for (int py = startY; py <= endY; ++py) {
      int dx = px - x;
      int dy = py - y;
      if (dx * dx + dy * dy <= radius * radius) {
        pixelData[py * (pitch / 4) + px] = brushColor;
      }
    }
  }
}

void UI::Draw_Line_To_Texture(int x1, int y1, int x2, int y2, int radius,
                              SDL_Color color) {
  void *pixels;
  int pitch;
  if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
    std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
    return;
  }

  uint32_t *pixelData = (uint32_t *)pixels;
  uint32_t brushColor =
      (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;

  int dx = std::abs(x2 - x1);
  int dy = std::abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    Draw_Circle_To_Pixels(pixelData, pitch, x1, y1, radius, brushColor);
    if (x1 == x2 && y1 == y2)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }

  SDL_UnlockTexture(texture);
}

void UI::Clear_Screen() {
  void *pixels;
  int pitch;
  if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
    std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
    return;
  }

  uint32_t *pixelData = (uint32_t *)pixels;
  const int rowPitch = pitch / 4;
  for (int y = 0; y < WINDOW_HEIGHT; ++y) {
    for (int x = 0; x < WINDOW_WIDTH; ++x) {
      pixelData[y * rowPitch + x] = BG_COLOR;
    }
  }
  SDL_UnlockTexture(texture);
}

void UI::Run() {
  bool running = true;
  while (running) {
    while (SDL_PollEvent(event)) {
      Event_Handler(*event, running);
      brush.Increase_Brush_Size(*event, *brush.Get_Brush_Struct());
    }
    brush.Update_Brush(brush.Get_Brush_Struct());

    SDL_SetRenderDrawColor(renderer, Window_Color.r, Window_Color.g,
                           Window_Color.b, Window_Color.a);
    SDL_RenderClear(renderer);

    if (Change_Background) {
      Clear_Screen();
      Change_Background = false;
      std::cout << "Reset background" << std::endl;
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    Draw_Color_Box();
    brush.Draw_Brush(renderer);

    if (brush.Get_Brush_Struct()->Is_Drawing) {
      int currX = brush.Get_Brush_Struct()->Center_X;
      int currY = brush.Get_Brush_Struct()->Center_Y;
      int radius = brush.Get_Brush_Struct()->Radius;
      SDL_Color color = brush.Get_Brush_Color();
      if (lastDrawX != -1) {
        Draw_Line_To_Texture(lastDrawX, lastDrawY, currX, currY, radius, color);
      } else {
        Draw_To_Texture(currX, currY, radius, color);
      }
      lastDrawX = currX;
      lastDrawY = currY;
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(FPS_DELAY);
  }
}

