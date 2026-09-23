#ifndef USER_DATA_H
#define USER_DATA_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 512

extern bool file_exist;
extern const char *filepath;

void Init_File(FILE **filepointer);
void Write_File(FILE **filepointer, float accuracy, double wps,
                int wrong_words);
void Read_File(FILE **filepointer);
float Word_Per_Second(int words, int seconds);
float Accuracy(const char *Text, char *Input);

#endif
