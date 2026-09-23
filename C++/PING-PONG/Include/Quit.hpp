#ifndef QUIT_HPP
#define QUIT_HPP
#include <SDL2/SDL.h>
#include <iostream>
static inline void Quit(SDL_Window *window, SDL_Renderer *renderer) {
  std::cout << "Quitting..." << std::endl;
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}
#endif
