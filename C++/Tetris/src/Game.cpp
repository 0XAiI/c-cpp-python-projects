#include <raylib.h>

#include <stdexcept>

#include "../include/Game.hpp"

#include "../include/Constants.hpp"

Game::Game() {
  InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
             Constants::SCREEN_TITLE.c_str());

  InitAudioDevice();
  SetTargetFPS(Constants::TARGET_FPS);

  int monitor = GetCurrentMonitor();

  int monitorWidth = GetMonitorWidth(monitor);
  int monitorHeight = GetMonitorHeight(monitor);

  center = Vector2{
      (static_cast<float>(monitorWidth - Constants::SCREEN_WIDTH) / 2),
      (static_cast<float>(monitorHeight - Constants::SCREEN_HEIGHT) / 2)};

  SetWindowPosition(center.x, center.y);
  SetTargetFPS(Constants::TARGET_FPS);

  if (!loadTetrisFont())
    throw std::runtime_error("Error loading tetris font");
  if (!loadTetrisIcon())
    throw std::runtime_error("Error loading tetris icon");
  if (!loadTetrisMusic())
    throw std::runtime_error("Error loading tetris music");
  if (!loadTetrisGameOverMusic())
    throw std::runtime_error("Error loading tetris game over music");

  SetWindowIcon(tetrisIcon);
  UnloadImage(tetrisIcon);

  gameStatus = MENU;
}

Game::~Game() {
  UnloadFont(tetrisFont);
  UnloadMusicStream(tetrisMusic);
  UnloadMusicStream(tetrisGameOverMusic);
}

void Game::run() {
  PlayMusicStream(tetrisMusic);

  bool running = true;
  float stop_timer = 0.0f;
  while (!WindowShouldClose() && running) {
    float dt = GetFrameTime();

    UpdateMusicStream(tetrisMusic);
    if (gameStatus == LOSE)
      UpdateMusicStream(tetrisGameOverMusic);
    switch (gameStatus) {
    case MENU:
      if (IsKeyPressed(KEY_ENTER)) {
        gameStatus = PLAYING;

        StopMusicStream(tetrisMusic);
      }
      break;

    case PLAYING:
      if (IsKeyPressed(KEY_ENTER)) {
        gameStatus = MENU;
        PlayMusicStream(tetrisMusic);
      }
      break;

    case LOSE:
      if (!IsMusicStreamPlaying(tetrisGameOverMusic))
        PlayMusicStream(tetrisGameOverMusic);
      if (tetromino.getGameOver()) {
        stop_timer += dt;
        if (stop_timer >= 1.5f)
          running = false;
      }
      break;
    }

    update(dt);
    BeginDrawing();
    ClearBackground(BLACK);

    render();

    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  data.updateHighscore(getScore());
}

void Game::drawNextTetromino() {
  TetrominoType nextType = tetromino.getNextTetrimino();
  auto &shapes = tetromino.getTetriminoShapes();
  auto &shape = shapes[nextType];

  const Vector2 boxPosition = {Constants::OFFSET + 280, Constants::OFFSET + 46};

  for (int row = 0; row < Constants::SHAPE_SIZE; row++) {
    for (int col = 0; col < Constants::SHAPE_SIZE; col++) {
      if (shape[row][col] != 0) {
        DrawRectangle(boxPosition.x + col * Constants::CELL_SIZE + 1,
                      boxPosition.y + row * Constants::CELL_SIZE + 1,
                      Constants::CELL_SIZE - 1, Constants::CELL_SIZE - 1,
                      board.getColors()[(int)nextType + 1]);
      }
    }
  }
}

void Game::drawGameOver() {
  if (tetromino.getGameOver()) {
    const Vector2 gameOverPosition = {
        Constants::SCREEN_WIDTH - Constants::OFFSET + 20, 20};

    DrawTextEx(tetrisFont, "Game Over", gameOverPosition,
               Constants::TETRIS_FONT_SIZE, Constants::TETRIS_FONT_SPACING,
               RED);
  }
}

void Game::render() {
  switch (gameStatus) {
  case MENU:
    DrawTextEx(tetrisFont, "PRESS ENTER TO START", {100, 300},
               Constants::TETRIS_FONT_SIZE, Constants::TETRIS_FONT_SPACING,
               WHITE);
    break;

  case PLAYING:
    board.drawBoard();
    board.drawBox();
    drawScore();
    tetromino.drawTetromino(board.getBoard(), board.getColors());
    drawNextTetromino();
    drawGameOver();
    break;

  case LOSE:

    DrawTextEx(tetrisFont, "GAME OVER", {250, 300}, Constants::TETRIS_FONT_SIZE,
               Constants::TETRIS_FONT_SPACING, RED);
    break;
  }
}

void Game::update(float &dt) {
  if (gameStatus != PLAYING)
    return;
  tetromino.updateTetromino(dt, board, board.getColors());
  if (tetromino.getGameOver()) {
    gameStatus = LOSE;
    return;
  }
  int cleared = board.clearRows();
  if (cleared > 0)
    score += cleared * 10;
}

bool Game::loadTetrisIcon() {
  tetrisIcon = LoadImage(Constants::TETRIS_ICON_PATH.c_str());
  return tetrisIcon.data != NULL;
}

bool Game::loadTetrisFont() {
  tetrisFont = LoadFont(Constants ::TETRIS_FONT_PATH.c_str());
  return tetrisFont.texture.id != 0;
}

bool Game::loadTetrisMusic() {
  tetrisMusic = LoadMusicStream(
      Constants::TETRIS_MUSIC_PATH[GetRandomValue(0, 1)].c_str());
  return tetrisMusic.stream.buffer != NULL;
}

bool Game::loadTetrisGameOverMusic() {
  tetrisGameOverMusic =
      LoadMusicStream(Constants::TETRIS_GAME_OVER_MUSIC_PATH.c_str());
  return tetrisGameOverMusic.stream.buffer != NULL;
}

void Game::drawScore() {

  const int width = 230;
  const int height = width;
  Vector2 scorePosition = {
      Constants::OFFSET + width,
      static_cast<int>(Constants::SCREEN_HEIGHT / 2 + Constants::OFFSET)};

  DrawTextEx(tetrisFont, TextFormat("Score : %d", score), scorePosition,
             Constants::TETRIS_FONT_SIZE, Constants::TETRIS_FONT_SPACING,
             WHITE);
}
