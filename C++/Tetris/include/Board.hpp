#ifndef BOARD_HPP
#define BOARD_HPP
#include "Constants.hpp"
#include <raylib.h>
#include <vector>
class Board {
public:
  Board();
  ~Board();
  void drawBoard();
  void drawBox();
  void setBoard(int row, int col, int value) { this->board[row][col] = value; }

  int clearRows();

  int (&getBoard()) [Constants::ROWS][Constants::COLS] { return board; }

  const std::vector<Color> &getColors() {
    return colors;
  }

  bool isRowFull(int row);

private:
  int board[Constants::ROWS][Constants::COLS];
  std::vector<Color> tetrisShapesColors();
  std::vector<Color> colors;
  Font tetrisFont;
};

#endif
