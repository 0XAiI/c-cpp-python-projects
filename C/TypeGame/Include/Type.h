#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PROMPT_LENGTH 2048
#define WORD_COUNT 100

extern const char *wordlist[WORD_COUNT];
extern unsigned int wordlist_length;
extern char prompt[MAX_PROMPT_LENGTH];

const char *get_random_word(void);
void generate_word_list(char *buffer, int count);

#endif
