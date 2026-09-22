#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include <iostream>

#include "../Include/Save.hpp"

namespace {
constexpr int MAX_SAVE_NUMBER = 15;
}

bool Save::Is_Save_Name_Valid() {
  std::string test_path =
      "../Saves/" + Save_Name + "_" + std::to_string(Save_Number) + ".png";

  FILE *file = fopen(test_path.c_str(), "r");
  if (file) {
    fclose(file);
    return false;
  }
  return true;
}

std::string Save::Set_Save_Name() {
  while (!Is_Save_Name_Valid() && Save_Number <= MAX_SAVE_NUMBER) {
    Save_Number++;
  }

  Save_Path =
      "../Saves/" + Save_Name + "_" + std::to_string(Save_Number) + ".png";
  return Save_Path;
}

void Save::Save_To_Png(SDL_Renderer *renderer, SDL_Texture *tex) {
  if (!renderer || !tex) {
    std::cerr << "Renderer or texture is null" << std::endl;
    return;
  }

  Save_Path = Set_Save_Name();

  int width, height;
  SDL_QueryTexture(tex, NULL, NULL, &width, &height);

  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32,
                                                        SDL_PIXELFORMAT_RGBA32);
  if (!surface) {
    std::cerr << "Create surface failed: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_Texture *old_target = SDL_GetRenderTarget(renderer);
  SDL_SetRenderTarget(renderer, tex);

  if (SDL_RenderReadPixels(renderer, NULL, surface->format->format,
                           surface->pixels, surface->pitch) != 0) {
    std::cerr << "ReadPixels failed: " << SDL_GetError() << std::endl;
    SDL_SetRenderTarget(renderer, old_target);
    SDL_FreeSurface(surface);
    return;
  }

  SDL_SetRenderTarget(renderer, old_target);

  if (Save_Number > MAX_SAVE_NUMBER) {
    SDL_FreeSurface(surface);
    std::cerr << "Too many saves" << std::endl;
    return;
  }

  if (IMG_SavePNG(surface, Save_Path.c_str()) != 0) {
    std::cerr << "Save PNG failed: " << SDL_GetError() << std::endl;
  } else {
    std::cout << "Saved to " << Save_Path << std::endl;
  }

  SDL_FreeSurface(surface);
}
