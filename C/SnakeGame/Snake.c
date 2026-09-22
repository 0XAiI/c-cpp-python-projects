#include <raylib.h>

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_SPEED 50
#define RADIUS 8.52f
#define SNAKE_BODY 1000
#define INITIAL_SPEED 0.81f
#define DIRECTION_SPEED 2.5f
#define SPEED_INCREMENT 0.001f

typedef struct {
  float pos_x;
  float pos_y;
  int width;
  int height;
} Snake;

typedef struct {
  Vector2 direction;
  Vector2 speed;
  Vector2 body[SNAKE_BODY];
  int body_length;
  float score;
  bool game_over;
} GameState;

void init_game_state(GameState *state, Snake *snake) {
  if (state == NULL || snake == NULL) {
    return;
  }

  snake->pos_x = WIDTH / 2.0f;
  snake->pos_y = HEIGHT / 2.0f;
  snake->width = 20;
  snake->height = 20;

  state->direction = (Vector2){0, 0};
  state->speed = (Vector2){INITIAL_SPEED, INITIAL_SPEED};
  state->body[0] = (Vector2){snake->pos_x, snake->pos_y};
  state->body_length = 1;
  state->score = 0;
  state->game_over = false;
}

Vector2 generate_random_position(int width, int height, Snake *snake,
                                 GameState *state) {
  Vector2 pos;
  bool valid_position = false;
  int max_attempts = 100;
  int attempts = 0;

  while (!valid_position && attempts < max_attempts) {
    pos.x = GetRandomValue(0, width - 50);
    pos.y = GetRandomValue(0, height - 50);

    valid_position = true;
    for (int i = 0; i < state->body_length; i++) {
      if (CheckCollisionPointRec(
              pos, (Rectangle){state->body[i].x, state->body[i].y,
                               (float)snake->width, (float)snake->height})) {
        valid_position = false;
        break;
      }
    }
    attempts++;
  }

  return pos;
}

void handle_input(GameState *state) {
  if (state == NULL) {
    return;
  }

  if (IsKeyPressed(KEY_DOWN) && state->direction.y == 0) {
    state->direction = (Vector2){0, DIRECTION_SPEED};
  }
  if (IsKeyPressed(KEY_UP) && state->direction.y == 0) {
    state->direction = (Vector2){0, -DIRECTION_SPEED};
  }
  if (IsKeyPressed(KEY_LEFT) && state->direction.x == 0) {
    state->direction = (Vector2){-DIRECTION_SPEED, 0};
  }
  if (IsKeyPressed(KEY_RIGHT) && state->direction.x == 0) {
    state->direction = (Vector2){DIRECTION_SPEED, 0};
  }
}

void update_game_state(GameState *state, Snake *snake, Vector2 *food) {
  if (state == NULL || snake == NULL || food == NULL) {
    return;
  }

  if (snake->pos_x < 0 || snake->pos_y < 0 ||
      snake->pos_x > WIDTH - snake->width ||
      snake->pos_y > HEIGHT - snake->height) {
    state->game_over = true;
    return;
  }

  snake->pos_x += state->direction.x * state->speed.x;
  snake->pos_y += state->direction.y * state->speed.y;

  for (int i = state->body_length - 1; i > 0; i--) {
    state->body[i] = state->body[i - 1];
  }
  state->body[0] = (Vector2){snake->pos_x, snake->pos_y};

  Rectangle snake_head = {snake->pos_x, snake->pos_y, (float)snake->width,
                          (float)snake->height};
  if (CheckCollisionCircleRec(*food, RADIUS, snake_head)) {
    state->score += 50;

    if (state->body_length < SNAKE_BODY) {
      state->body_length++;
      state->speed.x = INITIAL_SPEED + (state->score * SPEED_INCREMENT);
      state->speed.y = INITIAL_SPEED + (state->score * SPEED_INCREMENT);
    }

    *food = generate_random_position(WIDTH, HEIGHT, snake, state);
  }
}

void handle_game_over(GameState *state, Snake *snake) {
  if (state == NULL || snake == NULL) {
    return;
  }

  if (state->game_over) {
    DrawText("GAME OVER (PRESS ENTER TO CONTINUE)", 150, 100, 25, RED);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
      init_game_state(state, snake);
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) {
    }
  }
}

void draw_game(GameState *state, Snake *snake, Vector2 *food) {
  if (state == NULL || snake == NULL || food == NULL) {
    return;
  }

  DrawText(TextFormat("Score : %1.f", state->score), 0, 0, 25, WHITE);

  for (int i = 0; i < state->body_length; i++) {
    DrawRectangle(state->body[i].x, state->body[i].y, snake->width,
                  snake->height, GREEN);
  }

  DrawCircle(food->x, food->y, RADIUS, RED);

  if (state->game_over) {
    handle_game_over(state, snake);
  }
}

int main(void) {
  InitAudioDevice();
  SetRandomSeed(time(NULL));

  Snake snake;
  GameState state;
  init_game_state(&state, &snake);

  InitWindow(WIDTH, HEIGHT, "Snake Game");
  SetTargetFPS(60);

  Vector2 food = generate_random_position(WIDTH, HEIGHT, &snake, &state);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    handle_input(&state);

    if (!state.game_over) {
      update_game_state(&state, &snake, &food);
    }

    draw_game(&state, &snake, &food);

    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();

  return 0;
}
