#include <SFML/System/Time.hpp>
#include <cmath>

#include <algorithm>

#include "Visual.hpp"

namespace {
constexpr float MIN_LOUDNESS = 0.0f;
constexpr float MAX_LOUDNESS = 1.0f;
constexpr float BASE_COLOR_R = 0;
constexpr float BASE_COLOR_G = 150;
constexpr float BASE_COLOR_B = 200;
constexpr float COLOR_RANGE_G = 105;
constexpr float COLOR_RANGE_B = 55;
constexpr float MIN_HEIGHT = 100.0f;
constexpr float HEIGHT_MULTIPLIER = 300.0f;
constexpr float SMOOTHING_FACTOR = 0.9f;
constexpr float AMPLITUDE_FACTOR = 0.1f;
} // namespace

void Visual::Set_Visual() {
  for (int i = 0; i < BAR_COUNT; i++) {
    Bars[i].setSize(sf::Vector2f(BarWidth, 10.0f));
    Bars[i].setPosition(
        sf::Vector2f(static_cast<float>(i) * BarWidth,
                     static_cast<float>(HEIGHT) / 2.0f + 250.0f));
    Bars[i].setFillColor(sf::Color::Cyan);

    Reflect[i].setSize(sf::Vector2f(BarWidth - 2.0f, 5.0f));
    Reflect[i].setPosition(
        sf::Vector2f(static_cast<float>(i) * BarWidth,
                     static_cast<float>(HEIGHT) / 2.0f + 250.0f));
    Reflect[i].setFillColor(sf::Color(100, 255, 255, 100));
  }
}

sf::Color Visual::MakeColor(float loudness) {
  loudness = std::max(MIN_LOUDNESS, std::min(loudness, MAX_LOUDNESS));
  return sf::Color(
      static_cast<unsigned char>(BASE_COLOR_R),
      static_cast<unsigned char>(BASE_COLOR_G + loudness * COLOR_RANGE_G),
      static_cast<unsigned char>(BASE_COLOR_B + loudness * COLOR_RANGE_B));
}

float Visual::CalculateAmplitude(float time, int index) {
  const float baseFreq = 0.1f;
  const float freqStep = 0.05f;
  const float waveFreq = 5.0f;
  const float ampRange = 0.5f;

  float freq = baseFreq + (index * freqStep);
  float amplitude = ampRange + ampRange * std::sin(time * freq * waveFreq);
  return amplitude;
}

void Visual::Update_Visual(sf::Music &music) {
  const sf::Time offset = music.getPlayingOffset();
  const float time = offset.asSeconds();

  for (int i = 0; i < BAR_COUNT; i++) {
    const float amplitude = CalculateAmplitude(time, i);
    amplitudes[i] =
        SMOOTHING_FACTOR * amplitudes[i] + AMPLITUDE_FACTOR * amplitude;

    const float height = MIN_HEIGHT + amplitudes[i] * HEIGHT_MULTIPLIER;
    Bars[i].setSize(sf::Vector2f(BarWidth - 2.0f, -height));
    Bars[i].setFillColor(MakeColor(amplitudes[i]));
  }
}

void Visual::Draw_Visual(sf::RenderWindow &window) {
  for (int i = 0; i < BAR_COUNT; i++) {
    window.draw(Bars[i]);
  }
}
