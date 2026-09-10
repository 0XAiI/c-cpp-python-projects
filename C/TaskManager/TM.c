#include <raylib.h>

#include <stdbool.h>
#include <string.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 700
#define MAX_TASK_LEN 128
#define MAX_TASKS 12

typedef struct {
  char text[MAX_TASK_LEN];
  bool done;
} Task;

const Vector2 CHECK_RECTANGLE = {.x = 90, .y = 180};
const Vector2 WINDOW_REG = {.x = 50.f, .y = 25.f};
const Vector2 OFFSET = {.x = 150.f, .y = 150.f};

Task tasks[MAX_TASKS];
int task_count = 0;

void init_window(void);
void add_task(Font font);
void draw_tasks(Font font);
void remove_task(int index);
void clear_task_buffer(char *buffer, int *count);

int main(void) {
  init_window();
  return 0;
}

void clear_task_buffer(char *buffer, int *count) {
  if (buffer == NULL || count == NULL) {
    return;
  }
  memset(buffer, 0, MAX_TASK_LEN);
  *count = 0;
}

void init_window(void) {
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "To-Do List");
  SetTargetFPS(60);

  Font font = GetFontDefault();

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawRectangleLines(WINDOW_REG.x, WINDOW_REG.y, SCREEN_WIDTH - 100,
                       SCREEN_HEIGHT - 50, WHITE);
    DrawRectangle(WINDOW_REG.x, WINDOW_REG.y, SCREEN_WIDTH - 100,
                  SCREEN_HEIGHT - 50, (Color){150, 150, 150, 255});

    DrawTextEx(font, "TODO LIST",
               (Vector2){WINDOW_REG.x + 10, WINDOW_REG.y + 10}, 32, 2, WHITE);

    add_task(font);
    draw_tasks(font);

    EndDrawing();
  }

  CloseWindow();
}

void add_task(Font font) {
  static char new_task_text[MAX_TASK_LEN] = {0};
  static int letters_count = 0;

  int key = GetCharPressed();
  while (key > 0) {
    if (letters_count < MAX_TASK_LEN - 1 && key >= 32 && key <= 125) {
      new_task_text[letters_count++] = (char)key;
      new_task_text[letters_count] = '\0';
    }
    key = GetCharPressed();
  }

  if (letters_count > 0 && IsKeyPressed(KEY_BACKSPACE)) {
    new_task_text[--letters_count] = '\0';
  }

  float boxX = OFFSET.x - WINDOW_REG.x - 30;
  float boxY = OFFSET.y - WINDOW_REG.y - 30;
  float boxW = SCREEN_WIDTH - 142;
  float boxH = 50;

  DrawRectangleLines(boxX, boxY, boxW, boxH, DARKGRAY);
  DrawTextEx(font, new_task_text, (Vector2){boxX + 10, boxY + 15}, 20, 1,
             DARKGREEN);

  if (IsKeyPressed(KEY_ENTER) && letters_count > 0 && task_count < MAX_TASKS) {
    strncpy(tasks[task_count].text, new_task_text, MAX_TASK_LEN - 1);
    tasks[task_count].text[MAX_TASK_LEN - 1] = '\0';
    tasks[task_count].done = false;
    task_count++;

    clear_task_buffer(new_task_text, &letters_count);
  }
}

void draw_tasks(Font font) {
  for (int i = 0; i < task_count; i++) {
    int y = CHECK_RECTANGLE.y + i * 40;

    Rectangle checkbox = {CHECK_RECTANGLE.x, (float)y, 20, 20};
    DrawRectangleLinesEx(checkbox, 2, DARKGRAY);

    if (tasks[i].done) {
      DrawRectangleRec(checkbox, DARKGREEN);
    }

    DrawTextEx(font, tasks[i].text, (Vector2){checkbox.x + 30, (float)y + 2},
               20, 1, BLACK);

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, checkbox)) {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        tasks[i].done = !tasks[i].done;
      }
      if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        remove_task(i);
      }
    }
  }
}

void remove_task(int index) {
  if (index < 0 || index >= task_count) {
    return;
  }

  for (int i = index; i < task_count - 1; i++) {
    tasks[i] = tasks[i + 1];
  }

  task_count--;
}
