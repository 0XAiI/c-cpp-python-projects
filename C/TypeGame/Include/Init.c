#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Init.h"

unsigned int SCREEN_WIDTH = 1300;
unsigned int SCREEN_HEIGHT = 900;
char input[MAX_INPUT_CHARS + 1] = {0};
char Text[MAX_INPUT_CHARS + 1] = {0};
bool Finished = false;
int Wrong_Words = 0;
float seconds = 0;
int LetterCounter = 0;
Vector2 TextPosition = (Vector2){20, 100};
Vector2 InputPosition = (Vector2){20, 100};
Rectangle Box;
Color Box_Color;
Font font;

void Init_Game() {
  srand(time(NULL));
  Init_Level();
  Init_Box();
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TypeGame");
  Init_Font(&font);
  SetTargetFPS(60);
}

void Init_Window() {
  Init_Game();

  Color BackgroundColor = Init_Color(125, 125, 125, 0xff);
  double Start_Time = GetTime();
  while (!WindowShouldClose() && !Finished) {
    BeginDrawing();
    ClearBackground(BackgroundColor);
    Finished = Is_Text_Finished(Text, input);

    if (Exit_Window(Text, input)) {
      Finished = true;
      break;
    }
    Draw_Correct_Text();

    Draw_Box(Box, Box_Color);
    Draw_Cursor();
    Draw_Text();

    EndDrawing();
  }
  seconds = (GetTime() - Start_Time);
  if (seconds < 0.001f)
    seconds = 0.001f;

  UnloadFont(font);
  CloseWindow();
}

void Init_Box() {
  Box = (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
  Box_Color = Init_Color(200, 200, 200, 255);
}

void Draw_Correct_Text() {
  float fontSize = 22;
  float spacing = 0.8f;
  Color TextColor = Init_Color(150, 160, 170, 0xff);

  Vector2 basePos = TextPosition;
  float currentLineWidth = 0.0f;
  float rightMargin = 50.0f;
  Wrong_Words = 0;

  for (int i = 0; Text[i] != '\0'; i++) {
    char c = Text[i];
    char letter[2] = {c, '\0'};
    Vector2 size = MeasureTextEx(font, letter, fontSize, spacing);

    if (currentLineWidth + size.x > SCREEN_WIDTH - rightMargin) {
      basePos.x = TextPosition.x;
      basePos.y += 50;
      currentLineWidth = 0.0f;
    }

    DrawTextEx(font, letter, basePos, fontSize, spacing, TextColor);

    basePos.x += size.x;
    currentLineWidth += size.x;
  }

  basePos = InputPosition;
  currentLineWidth = 0.0f;

  for (int i = 0; i < LetterCounter && Text[i] != '\0'; i++) {
    char c = Text[i];
    Color col = (input[i] == c) ? GREEN : RED;
    bool IsCorrect = input[i] == c;
    if (!IsCorrect)
      Wrong_Words++;

    char letter[2] = {c, '\0'};
    Vector2 size = MeasureTextEx(font, letter, fontSize, spacing);

    if (currentLineWidth + size.x > SCREEN_WIDTH - rightMargin) {
      basePos.x = InputPosition.x;
      basePos.y += 50;
      currentLineWidth = 0.0f;
    }

    DrawTextEx(font, letter, basePos, fontSize, spacing, col);

    basePos.x += size.x;
    currentLineWidth += size.x;
  }
}
void Draw_Box(Rectangle Box, Color Box_Color) {
  DrawRectangleLinesEx(Box, 5, Box_Color);
}

void Init_Text(int level) {
  prompt[0] = '\0';

  generate_word_list(prompt, level);

  strncpy(Text, prompt, MAX_INPUT_CHARS);
  Text[MAX_INPUT_CHARS] = '\0';
}

bool Is_Text_Finished(const char *Text, char *input) {
  if (Text == NULL || input == NULL)
    return false;
  return strcmp(Text, input) == 0;
}

bool Exit_Window(const char *Text, char *input) {
  if (Text == NULL || input == NULL)
    return false;
  return (strlen(input) >= strlen(Text) && IsKeyPressed(KEY_ENTER));
}

void Draw_Text() {
  int Key = GetCharPressed();
  while (Key != 0) {
    if (Key >= 32 && Key <= 125 && LetterCounter < MAX_INPUT_CHARS) {
      input[LetterCounter] = (char)Key;
      LetterCounter++;
      input[LetterCounter] = '\0';
    }
    Key = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE) && LetterCounter > 0) {
    LetterCounter--;
    input[LetterCounter] = '\0';
  }

  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_BACKSPACE)) {
    LetterCounter = (LetterCounter > 5) ? LetterCounter - 5 : 0;
    input[LetterCounter] = '\0';
  }
}

void Draw_Cursor() {
  float fontSize = 22;
  float spacing = 0.8f;

  Vector2 cursorPos = InputPosition;

  float currentLineWidth = 0.0f;
  float rightMargin = 50.0f;

  for (int i = 0; i < LetterCounter; i++) {
    char c;

    if ((size_t)i < strlen(Text)) {
      c = Text[i];
    } else {
      c = (LetterCounter > 0 && strlen(Text) > 0) ? Text[strlen(Text) - 1]
                                                  : ' ';
    }

    char letter[2] = {c, '\0'};
    Vector2 size = MeasureTextEx(font, letter, fontSize, spacing);

    if (currentLineWidth + size.x > SCREEN_WIDTH - rightMargin) {
      cursorPos.x = InputPosition.x;
      cursorPos.y += 50;
      currentLineWidth = 0.0f;
    }

    cursorPos.x += size.x;
    currentLineWidth += size.x;
  }

  if (currentLineWidth > SCREEN_WIDTH - rightMargin) {
    cursorPos.x = InputPosition.x;
    cursorPos.y += 50;
  }

  bool showCursor = ((int)(GetTime() * 2) % 2) == 0;

  if (showCursor) {
    DrawRectangle(cursorPos.x, cursorPos.y, 2, fontSize, WHITE);
  }
}

void Init_Font(Font *font) {
  *font = LoadFont("../Font/FiraMonoNerdFontMono-Bold.otf");
  if (font->texture.id == 0) {
    printf("ERROR: Font not found, using default font\n");
    *font = GetFontDefault();
  } else {
    printf("Font loaded successfully\n");
  }
}

void Init_Level() {
  int choice = 0;
  int level = 1;
  bool stop = false;
  char line[64];

  do {
    printf("1-EASY\n2-MEDIUM\n3-HARD\n4-Quit\n");
    printf("Choose your level: ");

    if (fgets(line, sizeof(line), stdin) == NULL) {
      clearerr(stdin);
      printf("Invalid Choice\n");
      continue;
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
      line[len - 1] = '\0';

    char *endptr = NULL;
    long parsed = strtol(line, &endptr, 10);

    if (endptr == line || *endptr != '\0') {
      printf("Invalid Choice\n");
      continue;
    }

    choice = (int)parsed;

    switch (choice) {
    case 1:
      level = (rand() % (40 - 10 + 1)) + 10;
      stop = true;
      break;
    case 2:
      level = (rand() % (100 - 60 + 1)) + 60;
      stop = true;
      break;
    case 3:
      level = (rand() % (170 - 100 + 1)) + 100;
      stop = true;
      break;
    case 4:
      printf("Goodbye!\n");
      exit(EXIT_SUCCESS);
    default:
      printf("Invalid Choice\n");
      break;
    }
  } while (!stop);

  Init_Text(level);
}

void Run_TypeGame() {
  FILE *filepointer = NULL;
  Init_File(&filepointer);
  Init_Window();

  if (seconds <= 0)
    seconds = 0.001f;

  float accuracy = Accuracy(Text, input);
  double wps = Word_Per_Second(LetterCounter, seconds);

#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif

  Write_File(&filepointer, accuracy, wps, Wrong_Words);
  printf("View User File Data : \n");
  Read_File(&filepointer);
}
