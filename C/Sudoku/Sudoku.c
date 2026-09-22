#include <cs50.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID 9
#define SUB_GRID 3
#define MAX_ATTEMPTS 1000
#define CELLS_TO_FILL 25

int board[GRID][GRID];
int attempts = 0;

int get_random_number(void) { return (rand() % GRID) + 1; }

void init_board(int board[GRID][GRID]) {
  for (int i = 0; i < GRID; i++) {
    for (int j = 0; j < GRID; j++) {
      board[i][j] = 0;
    }
  }
}

bool is_box_valid(int board[GRID][GRID], int row, int col) {
  bool seen[10] = {false};

  for (int i = 0; i < SUB_GRID && row + i < GRID; i++) {
    for (int j = 0; j < SUB_GRID && col + j < GRID; j++) {
      int val = board[row + i][col + j];
      if (val == 0) {
        continue;
      }
      if (seen[val]) {
        return false;
      }
      seen[val] = true;
    }
  }
  return true;
}

bool are_boxes_valid(int board[GRID][GRID]) {
  for (int i = 0; i < GRID; i += SUB_GRID) {
    for (int j = 0; j < GRID; j += SUB_GRID) {
      if (!is_box_valid(board, i, j)) {
        return false;
      }
    }
  }
  return true;
}

bool is_valid(int board[GRID][GRID]) {
  for (int i = 0; i < GRID; i++) {
    bool seen[10] = {false};
    for (int j = 0; j < GRID; j++) {
      int val = board[i][j];
      if (val == 0) {
        continue;
      }
      if (seen[val]) {
        return false;
      }
      seen[val] = true;
    }
  }

  for (int j = 0; j < GRID; j++) {
    bool seen[10] = {false};
    for (int i = 0; i < GRID; i++) {
      int val = board[i][j];
      if (val == 0) {
        continue;
      }
      if (seen[val]) {
        return false;
      }
      seen[val] = true;
    }
  }

  return are_boxes_valid(board);
}

bool is_board_full(int board[GRID][GRID]) {
  for (int i = 0; i < GRID; i++) {
    for (int j = 0; j < GRID; j++) {
      if (board[i][j] == 0) {
        return false;
      }
    }
  }
  return true;
}

bool find_empty_cell(int board[GRID][GRID], int *row, int *col) {
  for (int i = 0; i < GRID; i++) {
    for (int j = 0; j < GRID; j++) {
      if (board[i][j] == 0) {
        *row = i;
        *col = j;
        return true;
      }
    }
  }
  return false;
}

bool solve_sudoku(int board[GRID][GRID]) {
  int row, col;

  if (!find_empty_cell(board, &row, &col)) {
    return true;
  }

  for (int num = 1; num <= GRID; num++) {
    board[row][col] = num;

    if (is_valid(board)) {
      if (solve_sudoku(board)) {
        return true;
      }
    }

    board[row][col] = 0;
  }

  return false;
}

void set_board(int board[GRID][GRID]) {
  init_board(board);
  attempts = 0;
  int count = 0;

  while (count < CELLS_TO_FILL && attempts < MAX_ATTEMPTS) {
    int row = rand() % GRID;
    int col = rand() % GRID;

    if (board[row][col] == 0) {
      int num = get_random_number();
      board[row][col] = num;

      if (is_valid(board)) {
        count++;
      } else {
        board[row][col] = 0;
      }
    }
    attempts++;
  }

  if (count < CELLS_TO_FILL) {
    printf("Warning: Could only place %d numbers after %d attempts\n", count,
           attempts);
  }
}

void print_board(int board[GRID][GRID]) {
  for (int i = 0; i < GRID; i++) {
    if (i % SUB_GRID == 0 && i != 0) {
      printf("------+-------+------\n");
    }
    for (int j = 0; j < GRID; j++) {
      if (j % SUB_GRID == 0 && j != 0) {
        printf("| ");
      }
      if (board[i][j] == 0) {
        printf(". ");
      } else {
        printf("%d ", board[i][j]);
      }
    }
    printf("\n");
  }
}

void display_menu(void) {
  printf("\n--- MENU ---\n");
  printf("1 - Generate Sudoku Board\n");
  printf("2 - Solve Sudoku Board\n");
  printf("3 - Print Sudoku Board\n");
  printf("4 - Exit\n");
}

void handle_menu_choice(char choice) {
  if (choice == CHAR_MAX) {
    printf("Exiting...\n");
    exit(EXIT_SUCCESS);
  }

  switch (choice) {
  case '1':
    set_board(board);
    printf("Board generated with %d numbers\n", CELLS_TO_FILL);
    break;

  case '2':
    if (!is_valid(board)) {
      printf("Invalid board configuration\n");
      break;
    }

    if (is_board_full(board)) {
      printf("Board is already full\n");
      break;
    }

    if (solve_sudoku(board)) {
      printf("Board solved successfully!\n");
    } else {
      printf("Unable to solve the board\n");
    }
    break;

  case '3':
    print_board(board);
    break;

  case '4':
    printf("Exiting...\n");
    exit(EXIT_SUCCESS);

  default:
    printf("Invalid choice. Please enter 1-4.\n");
    break;
  }
}

void menu(void) {
  while (true) {
    display_menu();
    char choice = get_char("Enter your choice: ");
    handle_menu_choice(choice);
  }
}

int main(void) {
  srand(time(NULL));

  menu();

  return EXIT_SUCCESS;
}
