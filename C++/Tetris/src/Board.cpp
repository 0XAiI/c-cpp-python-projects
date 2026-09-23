#include <raylib.h>

#include <vector>

#include "../include/Board.hpp"

Board::Board() {
  for (int i = 0; i < Constants::ROWS; i++) {
    for (int j = 0; j < Constants::COLS; j++) {
      board[i][j] = 0;
    }
  }
  colors = tetrisShapesColors();
}

Board::~Board() {}

void Board::drawBoard() {
  for (int row = 0; row < Constants::ROWS; row++) {
    for (int col = 0; col < Constants::COLS; col++) {
      DrawRectangle(col * Constants::CELL_SIZE + 1,
                    row * Constants::CELL_SIZE + 1, Constants::CELL_SIZE - 1,
                    Constants::CELL_SIZE - 1, colors[board[row][col]]);
    }
  }
}

bool Board::isRowFull(int row) {
  if (row < 0 || row >= Constants::ROWS) {
    printf("Invalid row: %d\n", row);
    return false;
  }
  for (int col = 0; col < Constants::COLS; col++) {
    if (board[row][col] == 0) {
      return false;
    }
  }
  return true;
}

int Board::clearRows() {
  int cleared = 0;
  for (int row = Constants::ROWS - 1; row >= 0; row--) {
    if (isRowFull(row)) {
      for (int r = row; r > 0; r--) {
        for (int c = 0; c < Constants::COLS; c++) {
          board[r][c] = board[r - 1][c];
        }
      }
      for (int c = 0; c < Constants::COLS; c++)
        board[0][c] = 0;
      cleared++;
      row++;
    }
  }
  return cleared;
}

void Board::drawBox() {
  const int width = 220;
  const int height = width;
  const int tickness = 5;
  const Vector2 boxPosition = {Constants::OFFSET + 280, Constants::OFFSET + 46};

  const Rectangle rec = {boxPosition.x - 60, boxPosition.y - 40,
                         static_cast<float>(width), static_cast<float>(height)};
  DrawRectangleLinesEx(rec, tickness, WHITE);
}

std::vector<Color> Board::tetrisShapesColors() {
  colors.push_back(GRAY);
  colors.push_back(RED);
  colors.push_back(GREEN);
  colors.push_back(BLUE);
  colors.push_back(YELLOW);
  colors.push_back(ORANGE);
  colors.push_back(PURPLE);
  colors.push_back(SKYBLUE);
  return colors;
}
