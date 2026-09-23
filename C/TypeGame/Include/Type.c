#include <stdio.h>
#include <string.h>

#include "Type.h"

#define WORD_COUNT 100

const char *wordlist[WORD_COUNT] = {
    "the",  "of",    "and",    "to",    "in",    "is",    "you",    "that",
    "it",   "he",    "was",    "for",   "on",    "are",   "as",     "with",
    "his",  "they",  "I",      "at",    "be",    "this",  "have",   "from",
    "or",   "one",   "had",    "by",    "word",  "but",   "not",    "what",
    "all",  "were",  "we",     "when",  "your",  "can",   "said",   "there",
    "use",  "an",    "each",   "which", "she",   "do",    "how",    "their",
    "if",   "will",  "up",     "other", "about", "out",   "many",   "then",
    "them", "these", "so",     "some",  "her",   "would", "make",   "like",
    "him",  "into",  "time",   "has",   "look",  "two",   "more",   "write",
    "go",   "see",   "number", "no",    "way",   "could", "people", "my",
    "than", "first", "water",  "been",  "call",  "who",   "oil",    "its",
    "now",  "find",  "long",   "down",  "day",   "did",   "get",    "come",
    "made", "may",   "part",   "over"};

unsigned int wordlist_length = WORD_COUNT;

char prompt[MAX_PROMPT_LENGTH] = {0};

const char *get_random_word(void) {
  int index = rand() % wordlist_length;
  return wordlist[index];
}

void generate_word_list(char *buffer, int count) {
  if (buffer == NULL) {
    fprintf(stderr, "Error: Buffer is NULL\n");
    return;
  }

  if (count <= 0) {
    fprintf(stderr, "Error: Invalid word count\n");
    return;
  }

  buffer[0] = '\0';
  size_t current_length = 0;

  for (int i = 0; i < count; i++) {
    const char *word = get_random_word();
    size_t word_len = strlen(word);

    if (current_length + word_len + 2 > MAX_PROMPT_LENGTH) {
      printf("Warning: Buffer full at word %d, stopping\n", i);
      break;
    }

    if (i > 0) {
      strcat(buffer, " ");
      current_length++;
    }

    strcat(buffer, word);
    current_length += word_len;
  }
}
