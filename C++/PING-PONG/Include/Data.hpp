
#ifndef DATA_HPP
#define DATA_HPP
#include "Game.hpp"
#include <cstdio>
#include <memory>
#include <stdio.h>
#include <string>

#define BUFFER_SIZE 1024

class Game_Data {
  friend class Game;

public:
  void Read_File_Data(const std::string &file_path);
  void Write_File_Data(const std::string &file_path,
                       std::unique_ptr<Game> &game);
  void Set_File_Empty(const std::string &file_path);

private:
  FILE *filepointer = nullptr;
};

#endif
