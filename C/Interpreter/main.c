#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 1024

bool ValidInput(const char *text, int size, int index) {
  if (size < 1 || text == NULL || index < 0 || index >= size) {
    return false;
  }

  char c = text[index];
  if (c == ' ') {
    return false;
  }

  return isdigit((int)c) || (c == '+' || c == '-' || c == '*' || c == '/');
}

int output_result(const char *text) {
  if (text == NULL) {
    fprintf(stderr, "Error: NULL input\n");
    return -1;
  }

  int result = 0;
  int len = strlen(text);

  if (len == 0) {
    fprintf(stderr, "Error: Empty input\n");
    return -1;
  }

  int i = 0;
  while (i < len && isdigit(text[i])) {
    result = result * 10 + (int)(text[i] - '0');
    i++;
  }

  while (i < len && text[i] != '\0') {
    char operation = text[i];
    int number = 0;
    i++;

    if (i >= len || !isdigit(text[i])) {
      fprintf(stderr, "Error: Invalid operator or missing number\n");
      return -1;
    }

    while (i < len && isdigit(text[i])) {
      number = number * 10 + (int)(text[i] - '0');
      i++;
    }

    switch (operation) {
    case '+':
      result += number;
      break;
    case '-':
      result -= number;
      break;
    case '*':
      result *= number;
      break;
    case '/':
      if (number == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return -1;
      }
      result /= number;
      break;
    default:
      fprintf(stderr, "Error: Invalid operator '%c'\n", operation);
      return -1;
    }
  }

  return result;
}

char *user_input_string(const char *text) {
  if (text == NULL) {
    fprintf(stderr, "Error: NULL input\n");
    return NULL;
  }

  int size = strlen(text);
  if (size < 1) {
    fprintf(stderr, "Error: Empty input\n");
    return NULL;
  }

  char *result = (char *)malloc(size + 1);
  if (result == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return NULL;
  }

  int j = 0;
  for (int i = 0; i < size; i++) {
    char c = text[i];
    if (ValidInput(text, size, i)) {
      result[j++] = c;
    }
  }
  result[j] = '\0';

  return result;
}

void process_input(void) {
  char text[MAX_BUFFER];
  printf("Interpreter running...\n");
  printf("Enter 'exit' or 'e' to quit\n");

  while (true) {
    printf(">>> ");

    if (fgets(text, sizeof(text), stdin) == NULL) {
      break;
    }

    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
      text[len - 1] = '\0';
    }

    if (strncmp(text, "exit", 4) == 0 || strncmp(text, "e", 1) == 0) {
      break;
    }

    char *input = user_input_string(text);
    if (input == NULL) {
      continue;
    }

    int result = output_result(input);
    if (result != -1) {
      printf("input without spacing: %s\n", input);
      printf("result: %d\n", result);
    }

    free(input);
  }
}

int main(void) {
  process_input();
  return EXIT_SUCCESS;
}
