
#include "User_Data.h"

bool file_exist = false;
const char *filepath = "../Include/UserData.txt";

void Init_File(FILE **filepointer) {
  if (filepointer == NULL) {
    fprintf(stderr, "Error: NULL file pointer\n");
    return;
  }

  *filepointer = fopen(filepath, "a+");
  if (*filepointer == NULL) {
    perror("Error creating file");
    file_exist = false;
  } else {
    file_exist = true;
    fclose(*filepointer);
    *filepointer = NULL;
  }
}

void Read_File(FILE **filepointer) {
  if (filepointer == NULL || *filepointer == NULL || !file_exist) {
    fprintf(stderr, "Error: Cannot read file\n");
    return;
  }

  char buffer[MAX];
  *filepointer = fopen(filepath, "r");
  if (*filepointer == NULL) {
    perror("Error opening file for reading");
    return;
  }

  while (fgets(buffer, sizeof(buffer), *filepointer)) {
    printf("%s", buffer);
  }

  fclose(*filepointer);
  *filepointer = NULL;
}

void Write_File(FILE **filepointer, float accuracy, double wps,
                int wrong_words) {
  if (filepointer == NULL || !file_exist) {
    fprintf(stderr, "Error: Cannot write to file\n");
    return;
  }

  *filepointer = fopen(filepath, "a");
  if (*filepointer == NULL) {
    perror("Error opening file for writing");
    return;
  }

  fprintf(*filepointer,
          "Accuracy : %.2f\nWordPerSecond: %.2f\nWrongWords: %d\n", accuracy,
          wps, wrong_words);

  fclose(*filepointer);
  *filepointer = NULL;
}

float Word_Per_Second(int words, int seconds) {
  if (seconds <= 0) {
    return 0.0f;
  }
  return ((float)words / seconds) * 10.0f;
}

float Accuracy(const char *Text, char *Input) {
  if (Text == NULL || Input == NULL) {
    return 0.0f;
  }

  int text_length = strlen(Text);
  int input_length = strlen(Input);

  if (input_length == 0 || text_length == 0) {
    return 0.0f;
  }

  int Correct_Words = 0;
  int min_length = (text_length < input_length) ? text_length : input_length;

  for (int i = 0; i < min_length; i++) {
    if (Input[i] == Text[i]) {
      Correct_Words++;
    }
  }

  return (Correct_Words / (float)input_length) * 100.0f;
}
