#ifndef COMMON_HPP
#define COMMON_HPP
#define WIDTH 700
#define HEIGHT 650
#include <raylib.h>

extern bool Is_Destroyed;
static bool Valid_Sound(Sound sound) { return (sound.frameCount > 0); }
#endif
