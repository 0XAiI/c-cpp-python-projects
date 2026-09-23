#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array>
#include <string>

namespace Constants {

constexpr unsigned int CELL_SIZE = 32;
constexpr unsigned int ROWS = 25;
constexpr unsigned int COLS = 15;
constexpr unsigned int OFFSET = 300;

constexpr unsigned int SCREEN_WIDTH = COLS * CELL_SIZE + OFFSET;
constexpr unsigned int SCREEN_HEIGHT = ROWS * CELL_SIZE;

constexpr int TARGET_FPS = 60;
constexpr int SHAPE_SIZE = 4;

const std::string SCREEN_TITLE = "Tetris Game";
const std::string TETRIS_FONT_PATH = "resources/font/Tetris.ttf";
const std::string TETRIS_ICON_PATH = "resources/logo/Tetris_Logo.png";
const std::string TETRIS_MUSIC_PATH[2] = {"resources/sounds/title.mp3",
                                          "resources/sounds/nutcracker-5.mp3"};
const std::string TETRIS_GAME_OVER_MUSIC_PATH =
    "resources/sounds/game-over.mp3";
const float TETRIS_FONT_SIZE = 38.0f;
const float TETRIS_FONT_SPACING = 5.0f;

const std::string TETRIS_HIGHSCORE_PATH = "data/highscores.txt";

} // namespace Constants

enum class TetrominoType { I = 0, O, T, S, Z, J, L };

namespace TetrisShapes {

using Shape =
    std::array<std::array<int, Constants::SHAPE_SIZE>, Constants::SHAPE_SIZE>;

constexpr Shape I = {
    {{{0, 0, 0, 0}}, {{1, 1, 1, 1}}, {{0, 0, 0, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape O = {
    {{{0, 0, 0, 0}}, {{0, 1, 1, 0}}, {{0, 1, 1, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape T = {
    {{{0, 0, 0, 0}}, {{0, 1, 0, 0}}, {{1, 1, 1, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape S = {
    {{{0, 0, 0, 0}}, {{0, 1, 1, 0}}, {{1, 1, 0, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape Z = {
    {{{0, 0, 0, 0}}, {{1, 1, 0, 0}}, {{0, 1, 1, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape J = {
    {{{0, 0, 0, 0}}, {{1, 0, 0, 0}}, {{1, 1, 1, 0}}, {{0, 0, 0, 0}}}};

constexpr Shape L = {
    {{{0, 0, 0, 0}}, {{0, 0, 1, 0}}, {{1, 1, 1, 0}}, {{0, 0, 0, 0}}}};

} // namespace TetrisShapes

#endif
