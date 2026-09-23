#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "MP.hpp"

#include "Visual.hpp"

namespace {
constexpr float VOLUME_STEP = 10.0f;
constexpr float MAX_VOLUME = 200.0f;
constexpr float MIN_VOLUME = 0.0f;
constexpr float DEFAULT_VOLUME = 20.0f;
constexpr float SPEED_NORMAL = 1.0f;
constexpr float SPEED_FAST = 1.5f;
constexpr float SPEED_SLOW = 0.5f;
} // namespace

void MPlayer::drawText(sf::RenderWindow &window) {
  if (!fontLoaded) {
    return;
  }

  text->setFont(font);
  text->setCharacterSize(30);
  text->setFillColor(sf::Color::White);
  text->setPosition(sf::Vector2f(310.0f, 9.0f));
  text->setString(std::to_string(static_cast<int>(Get_Volume() + 1)));

  if (muteMusic) {
    muteText->setFont(font);
    muteText->setCharacterSize(30);
    muteText->setFillColor(sf::Color::Red);
    muteText->setPosition(sf::Vector2f(10.0f, 8.0f));
    muteText->setString("MUTE");
    window.draw(*muteText);
  }

  if (isRunning) {
    playText->setFont(font);
    playText->setCharacterSize(30);
    playText->setFillColor(sf::Color::Green);
    playText->setPosition(sf::Vector2f(600.0f, 10.0f));
    playText->setString("PLAY");
    window.draw(*playText);
  } else {
    playText->setFont(font);
    playText->setCharacterSize(30);
    playText->setFillColor(sf::Color::Green);
    playText->setPosition(sf::Vector2f(600.0f, 10.0f));
    playText->setString("PAUSE");
    window.draw(*playText);
  }

  window.draw(*text);
}

void MPlayer::Increase_Volume() {
  const float currentVolume = music.getVolume();
  const float newVolume = std::min(currentVolume + VOLUME_STEP, MAX_VOLUME);
  music.setVolume(newVolume);

  if (newVolume >= MAX_VOLUME) {
    std::cout << "Volume : 200%(Max)" << std::endl;
  }
}

void MPlayer::Decrease_Volume() {
  const float currentVolume = music.getVolume();
  const float newVolume = std::max(currentVolume - VOLUME_STEP, MIN_VOLUME);
  music.setVolume(newVolume);

  if (newVolume <= MIN_VOLUME) {
    std::cout << "Volume : 0%(Min)" << std::endl;
  }
}

void MPlayer::Mute_Volume() {
  if (muteMusic) {
    music.setVolume(DEFAULT_VOLUME);
  } else {
    music.setVolume(MIN_VOLUME);
  }
  muteMusic = !muteMusic;
}

std::string MPlayer::Convert_MP4_To_Wave(const std::string &filepath) {
  wavFile = "temp.wav";
  isTempFile = true;
  const std::string command = "ffmpeg -y -i \"" + filepath + "\" " + wavFile;
  const int result = std::system(command.c_str());
  if (result != 0) {
    std::cerr << "Warning: ffmpeg conversion returned non-zero: " << result
              << std::endl;
  }
  std::ifstream check(wavFile);
  if (!check.good()) {
    std::cerr << "Error: ffmpeg produced no playable file" << std::endl;
    return "";
  }
  return wavFile;
}

float MPlayer::Get_Volume() { return music.getVolume(); }

void MPlayer::Increase_Speed() {
  if (!isRunning) {
    return;
  }

  if (currentSpeed == SPEED_SLOW) {
    music.setPitch(SPEED_NORMAL);
    currentSpeed = SPEED_NORMAL;
  } else {
    music.setPitch(SPEED_FAST);
    currentSpeed = SPEED_FAST;
  }
}

void MPlayer::Decrease_Speed() {
  if (!isRunning) {
    return;
  }

  if (currentSpeed == SPEED_FAST) {
    music.setPitch(SPEED_NORMAL);
    currentSpeed = SPEED_NORMAL;
  } else {
    music.setPitch(SPEED_SLOW);
    currentSpeed = SPEED_SLOW;
  }
}

bool MPlayer::If_File_MP4(const std::string &filepath) {
  return filepath.size() > 4 && filepath.substr(filepath.size() - 4) == ".mp4";
}

bool MPlayer::Init_Music(const std::string &musicpath) {
  std::string path = musicpath;

  if (If_File_MP4(path) || !music.openFromFile(path)) {
    path = Convert_MP4_To_Wave(path);
    if (path.empty()) {
      return false;
    }
  }

  if (!music.openFromFile(path)) {
    std::cerr << "Failed to load music file: " << path << std::endl;
    return false;
  }
  music.setVolume(DEFAULT_VOLUME);
  return true;
}

MPlayer::MPlayer() : isRunning(false), currentSpeed(SPEED_NORMAL) {}

MPlayer::~MPlayer() {
  if (music.getStatus() == sf::Music::Status::Playing) {
    music.stop();
  }

  if (isTempFile && !wavFile.empty()) {
    if (std::remove(wavFile.c_str()) == 0) {
      std::cout << "Cleaned up temporary file: " << wavFile << std::endl;
    } else {
      std::cerr << "Failed to remove temporary file: " << wavFile << std::endl;
    }
  }
}

void MPlayer::LoadAndPlay(const std::string &filepath) {
  if (!Init_Music(filepath)) {
    std::cerr << "Error initializing music" << std::endl;
    return;
  }

  currentTrack = filepath;
  music.play();
  isRunning = true;
}

void MPlayer::Run() {
#ifdef _WIN32
  std::system("cls");
#else
  std::system("clear");
#endif

  static const char *kFontCandidates[] = {
      "/usr/share/fonts/TTF/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/dejavu/DejaVuSans.ttf",
      "C:\\Windows\\Fonts\\arial.ttf",
      "/System/Library/Fonts/Helvetica.ttc",
  };
  for (const char *path : kFontCandidates) {
    if (font.openFromFile(path)) {
      fontLoaded = true;
      break;
    }
  }
  if (!fontLoaded) {
    std::cerr << "Warning: no system font found; HUD text disabled"
              << std::endl;
  }

  text = std::make_unique<sf::Text>(font);
  muteText = std::make_unique<sf::Text>(font);
  playText = std::make_unique<sf::Text>(font);

  window.create(sf::VideoMode(sf::Vector2u(700, 600)), "MUSIC PLAYER",
                sf::Style::Titlebar | sf::Style::Close);
  window.setPosition(windowPosition);
  window.setFramerateLimit(60);
  visual.Set_Visual();

  while (window.isOpen() && music.getStatus() != sf::Music::Status::Stopped) {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
        break;
      }
      if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>()) {
        switch (keyEvent->code) {
        case sf::Keyboard::Key::Left:
          Decrease_Speed();
          break;
        case sf::Keyboard::Key::Right:
          Increase_Speed();
          break;
        case sf::Keyboard::Key::M:
          Mute_Volume();
          break;
        case sf::Keyboard::Key::Up:
          Increase_Volume();
          break;
        case sf::Keyboard::Key::Down:
          Decrease_Volume();
          break;
        case sf::Keyboard::Key::Space:
          if (isRunning) {
            music.pause();
          } else {
            music.play();
          }
          isRunning = !isRunning;
          break;
        case sf::Keyboard::Key::Escape:
        case sf::Keyboard::Key::Q:
          std::cout << "Exiting ..." << std::endl;
          window.close();
          break;
        case sf::Keyboard::Key::Enter:
          music.setVolume(70.0f);
          break;
        default:
          break;
        }
      }
    }

    window.clear(backgroundColor);
    visual.Draw_Visual(window);
    drawText(window);
    visual.Update_Visual(music);
    window.display();
  }
}
