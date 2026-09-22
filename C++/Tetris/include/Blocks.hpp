#ifndef BLOCKS_HPP
#define BLOCKS_HPP
#include "Constants.hpp"
#include <raylib.h>

typedef struct BlocksStruct {

  bool touchGround;
  TetrominoType type;
  int rotation;
  Vector2 position;

} BlocksStruct;

#endif
