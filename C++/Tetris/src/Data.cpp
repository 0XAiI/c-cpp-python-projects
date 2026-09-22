#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../include/Data.hpp"

#include "../include/Constants.hpp"

int Data::loadHighscore() {
  std::ifstream infile(Constants::TETRIS_HIGHSCORE_PATH);
  int highscore = 0;
  std::string token;
  if (infile.is_open()) {
    while (infile >> token) {
      try {
        highscore = std::stoi(token);
      } catch (const std::exception &) {
      }
    }
  }
  return highscore;
}

void Data::saveHighscore(int score) {
  std::ofstream outfile(Constants::TETRIS_HIGHSCORE_PATH);
  if (outfile.is_open()) {
    outfile << "Score : " << score;
  }
}

void Data::updateHighscore(int newScore) {
  int currentHigh = loadHighscore();
  if (newScore > currentHigh) {
    saveHighscore(newScore);
    std::cout << "New High Score: " << newScore << "!\n";
  } else {
    std::cout << "Current High Score: " << currentHigh << "\n";
  }
}
