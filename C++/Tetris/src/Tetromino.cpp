#include <raylib.h>

#include <time.h>
#include <vector>

#include "../include/Tetromino.hpp"

Tetromino::Tetromino() {

  tetriminoShapes = {
      {TetrominoType::I, TetrisShapes::I}, {TetrominoType::O, TetrisShapes::O},
      {TetrominoType::T, TetrisShapes::T}, {TetrominoType::S, TetrisShapes::S},
      {TetrominoType::Z, TetrisShapes::Z}, {TetrominoType::J, TetrisShapes::J},
      {TetrominoType::L, TetrisShapes::L}};

  blocks.position = {5, 0};
  blocks.rotation = 0;
  SetRandomSeed(time(NULL));

  blocks.type = static_cast<TetrominoType>(GetRandomValue(0, 6));
  nextTetrimino = static_cast<TetrominoType>(GetRandomValue(0, 6));

  blocks.touchGround = false;
}

Tetromino::~Tetromino() {}

void Tetromino::drawTetromino(
    const int (&board)[Constants::ROWS][Constants::COLS],
    const std::vector<Color> &colors) {
  auto &shape = tetriminoShapes[blocks.type];
  for (int row = 0; row < Constants::SHAPE_SIZE; row++) {
    for (int col = 0; col < Constants::SHAPE_SIZE; col++) {
      if (shape[row][col] != 0)
        DrawRectangle((blocks.position.x + col) * Constants::CELL_SIZE + 1,
                      (blocks.position.y + row) * Constants::CELL_SIZE + 1,
                      Constants::CELL_SIZE - 1, Constants::CELL_SIZE - 1,
                      colors[(int)blocks.type + 1]);
    }
  }
}

bool Tetromino::checkCollision(
    const int (&board)[Constants::ROWS][Constants::COLS]) {
  auto &shape = tetriminoShapes[blocks.type];
  for (int row = 0; row < Constants::SHAPE_SIZE; row++) {
    for (int col = 0; col < Constants::SHAPE_SIZE; col++) {
      if (shape[row][col] != 0) {
        int br = (int)blocks.position.y + row;
        int bc = (int)blocks.position.x + col;
        if (br < 0 || br >= Constants::ROWS || bc < 0 || bc >= Constants::COLS)
          return true;
        if (board[br][bc] != 0)
          return true;
      }
    }
  }
  return false;
}

void Tetromino::rotationLogic(
    const int (&board)[Constants::ROWS][Constants::COLS]) {
  auto rotated = tetriminoShapes[blocks.type];
  for (int r = 0; r < Constants::SHAPE_SIZE; r++)
    for (int c = 0; c < Constants::SHAPE_SIZE; c++)
      rotated[r][c] =
          tetriminoShapes[blocks.type][Constants::SHAPE_SIZE - 1 - c][r];

  Vector2 kicks[] = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-1, -1}, {1, -1}};
  auto oldShape = tetriminoShapes[blocks.type];
  auto oldPos = blocks.position;
  bool success = false;

  tetriminoShapes[blocks.type] = rotated;
  for (auto &kick : kicks) {
    blocks.position = {oldPos.x + kick.x, oldPos.y + kick.y};
    if (!checkCollision(board)) {
      success = true;
      blocks.rotation = (int)(blocks.rotation + 1) % 4;
      break;
    }
  }
  if (!success) {
    tetriminoShapes[blocks.type] = oldShape;
    blocks.position = oldPos;
  }
}

void Tetromino::movements(
    float dt, const int (&board)[Constants::ROWS][Constants::COLS]) {
  if (blocks.touchGround)
    return;

  if (IsKeyPressed(KEY_LEFT)) {
    blocks.position.x--;
    if (checkCollision(board))
      blocks.position.x++;
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    blocks.position.x++;
    if (checkCollision(board))
      blocks.position.x--;
  }

  if (IsKeyPressed(KEY_UP)) {
    rotationLogic(board);
  }
  if (IsKeyPressed(KEY_SPACE)) {
    while (!checkCollision(board))
      blocks.position.y++;
    blocks.position.y--;
    blocks.touchGround = true;
  }
  if (IsKeyDown(KEY_DOWN)) {
    blocks.position.y++;
    if (checkCollision(board)) {
      blocks.position.y--;
      blocks.touchGround = true;
    }
  }

  setBoundaries();
}

void Tetromino::setBoundaries() {
  auto &shape = tetriminoShapes[blocks.type];

  int minCol = Constants::SHAPE_SIZE, maxCol = 0, maxRow = 0;
  for (int r = 0; r < Constants::SHAPE_SIZE; r++) {
    for (int c = 0; c < Constants::SHAPE_SIZE; c++) {
      if (shape[r][c] != 0) {
        if (c < minCol)
          minCol = c;
        if (c > maxCol)
          maxCol = c;
        if (r > maxRow)
          maxRow = r;
      }
    }
  }

  if (blocks.position.x + minCol < 0)
    blocks.position.x = -minCol;
  if (blocks.position.x + maxCol >= Constants::COLS)
    blocks.position.x = Constants::COLS - 1 - maxCol;
  if (blocks.position.y + maxRow >= Constants::ROWS) {
    blocks.position.y = Constants::ROWS - 1 - maxRow;
    blocks.touchGround = true;
  }
}

inline void Tetromino::spawnPiece() {
  blocks.position = {5, 0};
  blocks.rotation = 0;

  blocks.type = nextTetrimino;
  nextTetrimino = static_cast<TetrominoType>(GetRandomValue(0, 6));

  blocks.touchGround = false;
}

void Tetromino::updateTetromino(float &dt, Board &board,
                                const std::vector<Color> &colors) {

  fallTimer += dt;

  if (blocks.touchGround) {
    lockPiece(board.getBoard());
    spawnPiece();
    if (checkCollision(board.getBoard()))
      gameOver = true;
    return;
  }

  if (gameOver)
    return;

  movements(dt, board.getBoard());

  if (fallTimer >= 1.0f) {
    fallTimer = 0.0f;
    blocks.position.y++;
    if (checkCollision(board.getBoard())) {
      blocks.position.y--;
      blocks.touchGround = true;
    }
  }
}

void Tetromino::lockPiece(int (&board)[Constants::ROWS][Constants::COLS]) {
  auto &shape = tetriminoShapes[blocks.type];
  for (int row = 0; row < Constants::SHAPE_SIZE; row++) {
    for (int col = 0; col < Constants::SHAPE_SIZE; col++) {
      if (shape[row][col] != 0) {
        int boardRow = (int)blocks.position.y + row;
        int boardCol = (int)blocks.position.x + col;
        if (boardRow >= 0 && boardRow < Constants::ROWS && boardCol >= 0 &&
            boardCol < Constants::COLS) {
          board[boardRow][boardCol] = (int)blocks.type + 1;
        }
      }
    }
  }
}
