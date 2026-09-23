#include <raylib.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 10
#define TILE_SIZE 42
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 700
#define SCORE_PENALTY 20
#define MATCH_4_SCORE 50
#define MATCH_3_SCORE 30

const char RANDOM_LETTERS[] = "ABCDE";
const Vector2 GRID_OFFSET = {
    ((float)SCREEN_HEIGHT / 2) - 175,
    ((float)SCREEN_WIDTH / 2) - 275,
};

typedef struct {
  int score;
  bool select_block;
  bool winner;
  Vector2 mouse_position;
  Vector2 selected_tile;
} GameState;

bool is_board_empty(int board[GRID_SIZE][GRID_SIZE]);
int get_random_letter(void);
void create_window(int board[GRID_SIZE][GRID_SIZE]);
void initialize_board(int board[GRID_SIZE][GRID_SIZE]);
void handle_input(int board[GRID_SIZE][GRID_SIZE], GameState *state);
void check_matches(int board[GRID_SIZE][GRID_SIZE], GameState *state,
                   Music *music);
void draw_board(int board[GRID_SIZE][GRID_SIZE], GameState *state);
void cleanup_resources(Music *music, Texture2D *background);

int main(void) {
  srand(time(NULL));

  int board[GRID_SIZE][GRID_SIZE];
  initialize_board(board);
  create_window(board);

  return 0;
}

int get_random_letter(void) {
  int size = sizeof(RANDOM_LETTERS) - 1;
  return rand() % size;
}

void initialize_board(int board[GRID_SIZE][GRID_SIZE]) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      board[i][j] = RANDOM_LETTERS[get_random_letter()];
    }
  }
}

void cleanup_resources(Music *music, Texture2D *background) {
  if (music != NULL) {
    UnloadMusicStream(*music);
  }
  if (background != NULL) {
    UnloadTexture(*background);
  }
}

void create_window(int board[GRID_SIZE][GRID_SIZE]) {
  int score = 0;

  InitAudioDevice();
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Match Game");
  SetTargetFPS(60);

  Texture2D background = LoadTexture("./resources/background.png");
  Music music = LoadMusicStream("./resources/match_old.mp3");

  if (background.id == 0) {
    fprintf(stderr, "Failed to load background texture\n");
    cleanup_resources(&music, NULL);
    CloseWindow();
    CloseAudioDevice();
    return;
  }

  GameState state = {.score = score,
                     .select_block = false,
                     .winner = false,
                     .mouse_position = {0},
                     .selected_tile = {-1, -1}};

  while (!WindowShouldClose() && !state.winner) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(background, 0, 0, WHITE);

    state.mouse_position = GetMousePosition();

    handle_input(board, &state);
    check_matches(board, &state, &music);
    draw_board(board, &state);

    state.winner = is_board_empty(board);

    if (state.winner) {
      DrawText("YOU WON!", 175, 15, 25, GREEN);
    }

    DrawText(TextFormat("SCORE: %d", state.score), 15, 15, 25, YELLOW);

    EndDrawing();
  }

  if (state.winner) {
    WaitTime(2);
  }
  cleanup_resources(&music, &background);
  CloseWindow();
  CloseAudioDevice();

#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif

  if (state.winner) {
    printf("You won the game!\n");
    printf("Your score is %d\n", state.score);
  } else if (WindowShouldClose()) {
    printf("You lost the game\n");
  }
}

void handle_input(int board[GRID_SIZE][GRID_SIZE], GameState *state) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    state->select_block = true;
    Vector2 grid_origin = GRID_OFFSET;

    int x = (state->mouse_position.x - grid_origin.x) / TILE_SIZE;
    int y = (state->mouse_position.y - grid_origin.y) / TILE_SIZE;

    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
      state->selected_tile = (Vector2){(float)x, (float)y};
    }
  }

  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && state->select_block) {
    int x = (state->mouse_position.x - GRID_OFFSET.x) / TILE_SIZE;
    int y = (state->mouse_position.y - GRID_OFFSET.y) / TILE_SIZE;

    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
      board[x][y] = RANDOM_LETTERS[get_random_letter()];
    }

    if (state->score >= SCORE_PENALTY) {
      state->score -= SCORE_PENALTY;
    } else {
      state->score = 0;
    }
  }

  int i = state->selected_tile.x;
  int j = state->selected_tile.y;

  if (i >= 0 && j >= 0) {
    if (IsKeyPressed(KEY_LEFT) && i > 0) {
      char temp = board[i][j];
      board[i][j] = board[i - 1][j];
      board[i - 1][j] = temp;
    }

    if (IsKeyPressed(KEY_DOWN) && j < GRID_SIZE - 1) {
      char temp = board[i][j];
      board[i][j] = board[i][j + 1];
      board[i][j + 1] = temp;
    }

    if (IsKeyPressed(KEY_RIGHT) && i < GRID_SIZE - 1) {
      char temp = board[i][j];
      board[i][j] = board[i + 1][j];
      board[i + 1][j] = temp;
    }

    if (IsKeyPressed(KEY_UP) && j > 0) {
      char temp = board[i][j];
      board[i][j] = board[i][j - 1];
      board[i][j - 1] = temp;
    }
  }
}

void check_matches(int board[GRID_SIZE][GRID_SIZE], GameState *state,
                   Music *music) {
  int i = state->selected_tile.x;
  int j = state->selected_tile.y;

  if (i < 0 || j < 0)
    return;

  if (i < GRID_SIZE - 1 && j < GRID_SIZE - 1 && board[i][j] != ' ' &&
      board[i][j] == board[i + 1][j] && board[i][j] == board[i][j + 1] &&
      board[i][j] == board[i + 1][j + 1]) {

    StopMusicStream(*music);
    PlayMusicStream(*music);
    UpdateMusicStream(*music);

    state->score += MATCH_4_SCORE;

    board[i][j] = ' ';
    board[i + 1][j] = ' ';
    board[i][j + 1] = ' ';
    board[i + 1][j + 1] = ' ';
  } else if (i < GRID_SIZE - 2 && board[i][j] != ' ' &&
             board[i][j] == board[i + 1][j] && board[i][j] == board[i + 2][j]) {

    StopMusicStream(*music);
    PlayMusicStream(*music);
    UpdateMusicStream(*music);

    state->score += MATCH_3_SCORE;
    board[i][j] = board[i + 1][j] = board[i + 2][j] = ' ';
  } else if (j < GRID_SIZE - 2 && board[i][j] != ' ' &&
             board[i][j] == board[i][j + 1] && board[i][j] == board[i][j + 2]) {

    StopMusicStream(*music);
    PlayMusicStream(*music);
    UpdateMusicStream(*music);

    state->score += MATCH_3_SCORE;
    board[i][j] = board[i][j + 1] = board[i][j + 2] = ' ';
  }
}

void draw_board(int board[GRID_SIZE][GRID_SIZE], GameState *state) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      Rectangle tile = {i * TILE_SIZE + GRID_OFFSET.x,
                        j * TILE_SIZE + GRID_OFFSET.y, TILE_SIZE, TILE_SIZE};

      DrawRectangleLinesEx(tile, 2, LIGHTGRAY);

      Vector2 text_pos = {i * TILE_SIZE + GRID_OFFSET.x + 15,
                          j * TILE_SIZE + GRID_OFFSET.y + 10};

      if (i == state->selected_tile.x && j == state->selected_tile.y) {
        DrawRectangleLinesEx(
            (Rectangle){state->selected_tile.x * TILE_SIZE + GRID_OFFSET.x,
                        state->selected_tile.y * TILE_SIZE + GRID_OFFSET.y,
                        TILE_SIZE, TILE_SIZE},
            2, YELLOW);
      }

      if (board[i][j] != ' ') {
        DrawTextEx(GetFontDefault(), TextFormat("%c", board[i][j]), text_pos,
                   20, 2.5, WHITE);
      }
    }
  }
}

bool is_board_empty(int board[GRID_SIZE][GRID_SIZE]) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      if (board[i][j] != ' ') {
        return false;
      }
    }
  }
  return true;
}
