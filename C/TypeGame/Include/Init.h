#ifndef INIT_H
#define INIT_H

#include "Type.h"
#include "User_Data.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_CHARS MAX_PROMPT_LENGTH

void Init_Window();
static inline Color Init_Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (Color){r, g, b, a};
}

void Init_Game();
void Init_Box();
void Init_Text(int level);
void Init_Level();
bool Is_Text_Finished(const char *Text, char *input);
bool Exit_Window(const char *Text, char *input);
void Init_Font(Font *font);
void Draw_Box(Rectangle Box, Color Box_Color);
void Draw_Text();
void Draw_Cursor();
void Draw_Correct_Text();
void Run_TypeGame();

extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;
extern Rectangle Box;
extern Color Box_Color;
extern bool Finished;
extern char input[MAX_INPUT_CHARS + 1];
extern char Text[MAX_INPUT_CHARS + 1];
extern int LetterCounter;
extern Vector2 TextPosition;
extern Vector2 InputPosition;
extern Font font;

#endif
