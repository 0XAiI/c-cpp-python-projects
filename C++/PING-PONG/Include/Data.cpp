#include <cstdio>

#include <iostream>

#include "Data.hpp"

void Game_Data::Read_File_Data(const std::string &file_path) {

  filepointer = fopen(file_path.c_str(), "r");

  if (filepointer == nullptr) {
    std::cerr << "Error opening file: " << file_path << std::endl;
    return;
  }

  char buffer[BUFFER_SIZE];
  while (fgets(buffer, sizeof(buffer), filepointer)) {
    std::cout << buffer;
  }

  fclose(filepointer);
}

void Game_Data::Write_File_Data(const std::string &file_path,
                                std::unique_ptr<Game> &game) {
  filepointer = fopen(file_path.c_str(), "w");
  if (filepointer == nullptr) {
    std::cerr << "Error opening file: " << file_path << std::endl;
    return;
  }

  fprintf(filepointer, "\n===== Game Data =====\n");
  fprintf(filepointer, "Player 1: %d\n", game->ui->score.player_score1);
  fprintf(filepointer, "Player 2: %d\n", game->ui->score.player_score2);
  fprintf(filepointer, "=====================\n");

  fclose(filepointer);
}

void Game_Data::Set_File_Empty(const std::string &file_path) {
  filepointer = fopen(file_path.c_str(), "w");
  if (filepointer == NULL) {
    perror("fopen");
    return;
  }
  fclose(filepointer);
}
