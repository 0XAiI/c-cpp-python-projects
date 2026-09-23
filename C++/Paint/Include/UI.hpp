#define SDL_MAIN_HANDLED
#ifndef UI_HPP
#define UI_HPP

#include "Brush.hpp"
#include "Save.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <string>
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 750
#define WINDOW_TITLE "Paint App"

class UI {

public:
  UI();
  ~UI();

  bool Init_Window();
  void Run();
  void Event_Handler(SDL_Event &event, bool &running);
  void Init_Color_Box();
  void Draw_Color_Box();
  void Draw_To_Texture(int x, int y, int radius, SDL_Color color);

  void Draw_Line_To_Texture(int x1, int y1, int x2, int y2, int radius,
                            SDL_Color color);

  void Draw_Circle_To_Pixels(uint32_t *pixelData, int pitch, int x, int y,
                             int radius, uint32_t brushColor);
  void Load_Icon();
  void Clear_Screen();
  SDL_Window *Get_Window() { return window; }
  SDL_Renderer *Get_Renderer() { return renderer; }
  SDL_Texture *Get_Texture() { return texture; }

private:
  Brush brush;
  Save save;
  SDL_Event *event;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  std::vector<SDL_FRect> rects;
  static const std::string Icon_Path;
  SDL_Color Window_Color = {0, 0, 0, 255};
  int lastDrawX = -1;
  int lastDrawY = -1;
  bool Change_Background = false;
};

#endif
