#include <cs50.h>
#include <raylib.h>

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600
#define SCALE_FACTOR 50.0f
#define PLOT_SCALE 100.0f
#define TICK_SIZE 5

typedef enum {
  FUNCTION_SIN = 1,
  FUNCTION_COS,
  FUNCTION_TAN,
  FUNCTION_EXP,
  FUNCTION_LN,
  FUNCTION_X,
  FUNCTION_SQUARE,
  FUNCTION_ABS
} FunctionType;

void draw_axes(void);
void draw_sin(void);
void draw_cos(void);
void draw_tan(void);
void draw_exp(void);
void draw_ln(void);
void draw_x(void);
void draw_square(void);
void draw_abs(void);
void display_menu(void);
FunctionType get_function_choice(void);
void draw_function(FunctionType func);

int main(void) {
  Color background = {0, 0, 0, 255};

  display_menu();
  FunctionType function = get_function_choice();

  InitWindow(WIDTH, HEIGHT, "Functions");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(background);
    draw_axes();
    draw_function(function);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void display_menu(void) {
  printf("\n---- Function Plotter ----\n");
  printf("1 - sin(x)\n");
  printf("2 - cos(x)\n");
  printf("3 - tan(x)\n");
  printf("4 - e^x\n");
  printf("5 - ln(x)\n");
  printf("6 - x\n");
  printf("7 - x^2\n");
  printf("8 - |x|\n");
}

FunctionType get_function_choice(void) {
  int choice;
  do {
    choice = get_int("Enter your choice (1-8): ");
    if (choice == INT_MAX) {
      exit(EXIT_SUCCESS);
    }
    if (choice < 1 || choice > 8) {
      printf("Invalid choice. Please enter a number between 1 and 8.\n");
    }
  } while (choice < 1 || choice > 8);

  return (FunctionType)choice;
}

void draw_function(FunctionType func) {
  switch (func) {
  case FUNCTION_SIN:
    DrawText("sin(x)", 15, 15, 25, WHITE);
    draw_sin();
    break;
  case FUNCTION_COS:
    DrawText("cos(x)", 15, 15, 25, WHITE);
    draw_cos();
    break;
  case FUNCTION_TAN:
    DrawText("tan(x)", 15, 15, 25, WHITE);
    draw_tan();
    break;
  case FUNCTION_EXP:
    DrawText("e^x", 15, 15, 25, WHITE);
    draw_exp();
    break;
  case FUNCTION_LN:
    DrawText("ln(x)", 15, 15, 25, WHITE);
    draw_ln();
    break;
  case FUNCTION_X:
    DrawText("x", 15, 15, 25, WHITE);
    draw_x();
    break;
  case FUNCTION_SQUARE:
    DrawText("x^2", 15, 15, 25, WHITE);
    draw_square();
    break;
  case FUNCTION_ABS:
    DrawText("|x|", 15, 15, 25, WHITE);
    draw_abs();
    break;
  }
}

void draw_axes(void) {
  DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, WHITE);
  DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, WHITE);

  for (int i = -WIDTH / 2; i <= WIDTH / 2; i += 50) {
    DrawLine(WIDTH / 2 + i, HEIGHT / 2 - TICK_SIZE, WIDTH / 2 + i,
             HEIGHT / 2 + TICK_SIZE, WHITE);
  }

  for (int i = -HEIGHT / 2; i <= HEIGHT / 2; i += 50) {
    DrawLine(WIDTH / 2 - TICK_SIZE, HEIGHT / 2 + i, WIDTH / 2 + TICK_SIZE,
             HEIGHT / 2 + i, WHITE);
  }
}

void draw_sin(void) {
  for (int px = 0; px < WIDTH; px++) {
    float x = (px - (float)WIDTH / 2) / SCALE_FACTOR;
    float y = sinf(x);
    int py = HEIGHT / 2 - (int)(y * PLOT_SCALE);

    if (py >= 0 && py < HEIGHT) {
      DrawPixel(px, py, RED);
    }
  }
}

void draw_cos(void) {
  for (int px = 0; px < WIDTH; px++) {
    float x = (px - (float)WIDTH / 2) / SCALE_FACTOR;
    float y = cosf(x);
    int py = HEIGHT / 2 - (int)(y * PLOT_SCALE);

    if (py >= 0 && py < HEIGHT) {
      DrawPixel(px, py, RED);
    }
  }
}

void draw_tan(void) {
  for (int px = 0; px < WIDTH; px++) {
    float x = (px - (float)WIDTH / 2) / SCALE_FACTOR;
    float y = tanf(x);

    if (y > 10.0f)
      y = 10.0f;
    if (y < -10.0f)
      y = -10.0f;

    int py = HEIGHT / 2 - (int)(y * PLOT_SCALE);

    if (py >= 0 && py < HEIGHT) {
      DrawPixel(px, py, RED);
    }
  }
}

void draw_exp(void) {
  const float scale_x = 10.0f;
  const float scale_y = 20.0f;
  const int origin_x = WIDTH / 2;
  const int origin_y = HEIGHT / 2;

  for (int px = 0; px < WIDTH; px++) {
    float x = (px - origin_x) / scale_x;
    float y = expf(x);
    float py = origin_y - y * scale_y;

    if (py >= 0 && py < HEIGHT) {
      DrawPixel(px, (int)py, RED);
    }
  }
}

void draw_ln(void) {
  const float scale_x = 40.0f;
  const float scale_y = 20.0f;
  const int origin_x = WIDTH / 2;
  const int origin_y = HEIGHT / 2;

  float prev_x = origin_x + 0.01f * scale_x;
  float prev_y = origin_y - logf(0.01f) * scale_y;

  for (float x = 0.02f; x <= 10; x += 0.01f) {
    float y = logf(x);
    float sx = origin_x + x * scale_x;
    float sy = origin_y - y * scale_y;

    DrawLine((int)prev_x, (int)prev_y, (int)sx, (int)sy, RED);

    prev_x = sx;
    prev_y = sy;
  }
}

void draw_x(void) {
  for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
    int start_x = WIDTH / 2 + x;
    int start_y = HEIGHT / 2 - x;
    int end_x = WIDTH / 2 + x + 1;
    int end_y = HEIGHT / 2 - (x + 1);

    DrawLine(start_x, start_y, end_x, end_y, RED);
  }
}

void draw_square(void) {
  for (int x = -HEIGHT / 2; x < HEIGHT / 2; x++) {
    int y = x * x / 100;
    int screen_x = x + WIDTH / 2;
    int screen_y = HEIGHT / 2 - y;

    if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 &&
        screen_y < HEIGHT) {
      DrawPixel(screen_x, screen_y, RED);
    }
  }
}

void draw_abs(void) {
  for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
    int y = abs(x);
    int screen_x = x + WIDTH / 2;
    int screen_y = -y + HEIGHT / 2;

    if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 &&
        screen_y < HEIGHT) {
      DrawPixel(screen_x, screen_y, RED);
    }
  }
}
