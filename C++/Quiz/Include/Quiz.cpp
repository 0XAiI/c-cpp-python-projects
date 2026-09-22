#include <raylib.h>

#include <string>

#include "Quiz.hpp"

namespace {
constexpr int NUM_QUESTIONS = 10;
constexpr int NUM_CHOICES = 4;
constexpr int FRAME_INCREMENT = 3;
constexpr float TEXT_SPEED = 35.0f;
}

bool Quiz::Time_Finished = false;
bool Quiz::QuizAnswered = false;
bool Quiz::Collision = false;
std::string Quiz::question[10] = {" "};
std::string Quiz::choices[10][4] = {" "};
int Quiz::frameCounter = 0;
size_t Quiz::Index = 0;
Rectangle Quiz::Blocks_Pos[4] = {
    Rectangle{120, 580, 65, 65}, Rectangle{270, 580, 65, 65},
    Rectangle{420, 580, 65, 65}, Rectangle{570, 580, 65, 65}};
size_t Quiz::Score = 0;

Font Quiz::gameFont = {0, 0, 0, 0, 0, 0};
bool Quiz::fontLoaded = false;
bool Quiz::initialized = false;

void Quiz::LoadGameFont() {
  if (!fontLoaded) {
    gameFont = LoadFont("../Font/Font1.ttf");
    fontLoaded = true;
  }
}

void Quiz::UnloadGameFont() {
  if (fontLoaded) {
    UnloadFont(gameFont);
    fontLoaded = false;
  }
}

bool Quiz::IsCollision(Vector2 obj1, Vector2 obj2) {
  Rectangle rec1 = {obj1.x, obj1.y, 48, 48};
  Rectangle rec2 = {obj2.x, obj2.y, 65, 65};
  return CheckCollisionRecs(rec1, rec2);
}

bool Quiz::IsCorrectAnswer(Vector2 obj1, size_t Index) {
  static const Vector2 correctPositions[10] = {
      {120, 580}, {420, 580}, {270, 580}, {570, 580}, {570, 580},
      {270, 580}, {420, 580}, {120, 580}, {120, 580}, {270, 580}};

  if (Index >= 10)
    return false;
  return IsCollision(obj1, correctPositions[Index]);
}

void Quiz::Draw_Blocks(Vector2 obj1) {
  int blockIndex = 0;
  if (Collision == false) {
    for (int i = 120; i < GetScreenWidth() - BLOCK_SIZE && blockIndex < 4;
         i += BLOCK_SIZE, blockIndex++) {
      const Vector2 obj2 = Vector2{(float)i, 580};
      Blocks_Pos[blockIndex] = Rectangle{obj2.x, obj2.y, 65, 65};
      if (IsCollision(obj1, obj2)) {
        Collision = true;
        QuizAnswered = true;
        if (IsCorrectAnswer(obj1, Index))
          Score++;
        break;
      }
    }
    for (int i = 120; i < GetScreenWidth() - BLOCK_SIZE; i += BLOCK_SIZE)
      DrawRectangleLines(i, 580, 65, 65, GREEN);
  }
}

bool Quiz::Check_Font_Loaded(const Font font) {
  if (font.texture.id == 0) {
    std::cout << "Font not loaded" << std::endl;
    return false;
  }
  return true;
}

void Quiz::Draw_Choices() {
  static const char *choices_labels[] = {"A", "B", "C", "D"};
  int blockIndex = 0;

  for (int i = 120; i < GetScreenWidth() - BLOCK_SIZE && blockIndex < 4;
       i += BLOCK_SIZE) {
    const Vector2 textPos = {static_cast<float>(i + 20), 585.0f};

    if (Collision) {
      DrawTextEx(gameFont, TextFormat("%s", choices_labels[blockIndex]),
                 textPos, 50, 2, GREEN);
    } else if (Check_Font_Loaded(gameFont)) {
      DrawTextEx(gameFont, TextFormat("%s", choices_labels[blockIndex]),
                 textPos, 50, 2, GREEN);
    } else {
      DrawText("Font not loaded Use Default Font", 10, 10, 20, RED);
    }
    blockIndex++;
  }
}

void Quiz::Update_Choices() {
  static const char *choiceLabels[] = {"A", "B", "C", "D"};
  if (Index < 10) {
    for (int i = 0; i < 4; ++i) {
      const Vector2 Choice_Pos = {50.0f + static_cast<float>(i * 170), 520.0f};
      DrawTextEx(
          gameFont,
          TextFormat("%s: %s", choiceLabels[i], choices[Index][i].c_str()),
          Choice_Pos, 20, 2, WHITE);
    }
  }
  if (GetQuizAnswered()) {
    QuizAnswered = true;
    Index++;
  }
}

void Quiz::SetQuestion() {
  if (initialized)
    return;

  static const char *Temp_Questions[10] = {
      "What is the capital of France?",
      "2! + 4! = ?",
      "What is the largest planet in our solar system?",
      "Who wrote is the author of 'Ariel'?",
      "What is the chemical symbol for water?",
      "What is the fastest land animal?",
      "What is the smallest prime number?",
      "What is the hardest natural substance on Earth?",
      "What is the main ingredient in guacamole?",
      "What is the largest mammal in the world?"};
  for (int i = 0; i < 10; i++) {
    question[i] = Temp_Questions[i];
  }
  initialized = true;
}

void Quiz::SetChoices() {
  static const char *Temp_Choices[10][4] = {
      {"Paris", "London", "Berlin", "Madrid"},
      {"24", "22", "26", "18"},
      {"Mars", "Jupiter", "Earth", "Saturn"},
      {"Dickens", "Kafka", "Austen", "Plath"},
      {"NaCl", "CO2", "O2", "H2O"},
      {"Lion", "Cheetah", "Elephant", "Kangaroo"},
      {"0", "3", "2", "1"},
      {"Diamond", "Ruby", "Emerald", "Graphite"},
      {"Avocado", "Tomato", "Onion", "Pepper"},
      {"Walrus", "BlueWhale", "Camel", "Rhino"}};
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 4; ++j) {
      choices[i][j] = Temp_Choices[i][j];
    }
  }
}

void Quiz::Draw_Question() {
  SetQuestion();
  if (Check_Font_Loaded(gameFont)) {
    for (int i = 0; i < 10; i++) {
      std::string q = Quiz::question[i];
      const Vector2 QuestionPos = {10.0f, static_cast<float>(50 + i * 30)};
      if (IsKeyDown(KEY_SPACE)) {
        frameCounter += FRAME_INCREMENT;
      } else
        frameCounter++;
      DrawTextEx(gameFont,
                 TextSubtext(TextFormat("%i:%s", i + 1, q.c_str()), 0,
                             frameCounter / TEXT_SPEED),
                 QuestionPos, 25, 2, WHITE);
    }
  } else {
    DrawText("Font not loaded Use Default Font", 10, 10, 20, RED);
  }
}

void Timer(void) {
  static float time = 60.5f;

  const Vector2 TimePos = {static_cast<float>(GetScreenWidth() - 480), 475};
  if (!Quiz::fontLoaded) {
    std::cout << "Font not loaded" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!Quiz::Time_Finished) {
    time -= GetFrameTime();
    if (time <= 0.0f) {
      time = 0;
      Quiz::Time_Finished = true;
    }
    DrawTextEx(Quiz::gameFont, TextFormat("Timer:%i", (int)time), TimePos, 35,
               2, YELLOW);
  }
}
