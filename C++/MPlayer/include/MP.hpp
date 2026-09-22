#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

#include "Visual.hpp"

class MPlayer {
public:
  MPlayer();
  ~MPlayer();
  MPlayer(const MPlayer &) = delete;
  MPlayer &operator=(const MPlayer &) = delete;

  void Run();
  bool Init_Music(const std::string &musicpath);
  static bool If_File_MP4(const std::string &filepath);
  std::string Convert_MP4_To_Wave(const std::string &filepath);
  void LoadAndPlay(const std::string &filepath);
  void Increase_Volume();
  void Decrease_Volume();
  void Increase_Speed();
  void Decrease_Speed();
  void Mute_Volume();
  float Get_Volume();

private:
  void drawText(sf::RenderWindow &window);

  sf::Font font;
  bool fontLoaded = false;
  std::unique_ptr<sf::Text> text;
  std::unique_ptr<sf::Text> muteText;
  std::unique_ptr<sf::Text> playText;
  Visual visual;
  const sf::Vector2i windowPosition = {350, 100};
  const sf::Color backgroundColor = {20, 20, 30, 255};
  std::string currentTrack;
  sf::RenderWindow window;
  sf::Music music;
  bool isRunning = false;
  bool muteMusic = false;
  float currentSpeed = 1.0f;
  std::string wavFile;
  bool isTempFile = false;
};
