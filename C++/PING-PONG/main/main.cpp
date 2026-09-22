#define SDL_MAIN_HANDLED
#include <cstdlib>

#include <iostream>

#include "../Include/Data.hpp"
#include "../Include/Game.hpp"

const std::string file_path = "../Data/data.txt";

static inline void Print_Winner(const std::string &winner) {
  std::cout << "Winner: " << winner << std::endl;
}

int main() {
#ifdef _WIN32
  std::system("cls");
#else
  std::system("clear");
#endif

  Game_Data data;

  std::unique_ptr<Game> game = std::make_unique<Game>();

  data.Read_File_Data(file_path);

  game->Main_Loop();

  if (game->GetPlayer1Score() == game->GetPlayer2Score()) {
    printf("Winner: %s\n", "Tie");
  }

  else {
    Print_Winner(game->Check_Winner() ? "Player 1" : "Player 2");
  }

  data.Set_File_Empty(file_path);
  data.Write_File_Data(file_path, game);

  return EXIT_SUCCESS;
}
