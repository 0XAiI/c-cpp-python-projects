#ifndef QUIZ_HPP
#define QUIZ_HPP

#include <iostream>
#include <raylib.h>

constexpr int BLOCK_SIZE = 150;

class Quiz {
public:
  static size_t Score;
  static bool QuizAnswered;
  static bool Collision;
  static bool Time_Finished;
  static Font gameFont;
  static bool fontLoaded;

  static void LoadGameFont();
  static void UnloadGameFont();

  void SetQuestion();
  void SetChoices();
  void Draw_Choices();
  void Update_Choices();
  bool IsCorrectAnswer(Vector2 obj1, size_t Index);
  void Draw_Question();
  void Draw_Blocks(Vector2 obj1);
  bool Check_Font_Loaded(Font font);
  bool IsCollision(Vector2 obj1, Vector2 obj2);
  bool GetQuizAnswered() const { return QuizAnswered; }

private:
  static int frameCounter;
  static size_t Index;
  static Rectangle Blocks_Pos[4];
  static std::string question[10];
  static std::string choices[10][4];
  static bool initialized;
};

void Timer(void);

#endif
