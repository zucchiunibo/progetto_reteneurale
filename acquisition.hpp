#ifndef ACQUISITION_HPP
#define ACQUISITION_HPP

#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

namespace img {

std::vector<sf::Color>
getPixels(const sf::Image& source) { // returns vector of pixels' color
  auto pixelArray{
      source.getPixelsPtr()}; // pointer to read-only color array (R,G,B,Alpha)
  auto size = source.getSize();
  auto n    = size.x * size.y; // number of pixels
  std::vector<sf::Color> pixels{};
  for (int i{0}; i < n * 4; i += 4) {
    pixels.push_back({pixelArray[i], pixelArray[i + 1], pixelArray[i + 2],
                      pixelArray[i + 3]});
  }
  // controlla che il numero dei pixel combacia
  return pixels;
}

sf::Image resize(const sf::Image& source) { // bilinear interpolation
  int N{64};                                // size of new image
  auto sourceSize{source.getSize()};
  auto sourceX{sourceSize.x};
  auto sourceY{sourceSize.y};

  float xConv{static_cast<float>(sourceX - 1) / (N - 1)};
  float yConv{static_cast<float>(sourceY - 1) / (N - 1)}; // conversion factors

  sf::Image resized;
  resized.create(N, N); // sets resized size to NxN

  for (int x{0}; x < N; ++x) {
    for (int y{0}; y < N; ++y) { // coordinates in resized (to fill image)
      float xF{x * xConv};
      float yF{y * yConv}; // equivalent coordinates in source

      int x1{static_cast<int>(xF)};
      int y1{static_cast<int>(yF)}; // top left pixel

      int x2{std::min(x1 + 1, static_cast<int>(sourceX - 1))};
      int y2{std::min(
          y1 + 1, static_cast<int>(
                      sourceY - 1))}; // down right pixel (accounting borders)

      sf::Color Q11{source.getPixel(x1, y1)};
      sf::Color Q21{source.getPixel(x2, y1)};
      sf::Color Q12{source.getPixel(x1, y2)};
      sf::Color Q22{source.getPixel(x2, y2)}; // gets colors from source

      float xWeight{xF - x1};
      float yWeight{yF - y1}; // linear interpolation weights

      auto linErp = [](sf::Uint8 c1, sf::Uint8 c2, float weight) {
        return static_cast<sf::Uint8>(c1 * (1 - weight) + c2 * weight);
      }; // lambda of linear interpolation

      sf::Color C1{linErp(Q11.r, Q21.r, xWeight), linErp(Q11.g, Q21.g, xWeight),
                   linErp(Q11.b, Q21.b, xWeight),
                   linErp(Q11.a, Q21.a, xWeight)}; // Q11-Q21 linear interp

      sf::Color C2{linErp(Q12.r, Q22.r, xWeight), linErp(Q12.g, Q22.g, xWeight),
                   linErp(Q12.b, Q22.b, xWeight),
                   linErp(Q12.a, Q22.a, xWeight)}; // Q12-Q22 linear interp

      sf::Color C{linErp(C1.r, C2.r, yWeight), linErp(C1.g, C2.g, yWeight),
                  linErp(C1.b, C2.b, yWeight),
                  linErp(C1.a, C2.a, yWeight)}; // C1-C2 linear interp

      resized.setPixel(x, y, C);
    }
  }
  return resized;
}

std::vector<int>
binaryConvert(sf::Image source) { // converts image to binary
  auto pixels{getPixels(source)};
  std::vector<int> binary{};

  for (int i{0}; i < pixels.size(); ++i) {
    int colorAvg{
        static_cast<int>((pixels[i].r + pixels[i].b + pixels[i].g) / 3)};
    if (colorAvg >= 127) {
      binary.push_back(+1);
    } else {
      binary.push_back(-1);
    }
  }
  return binary;
}

sf::Image formImage(std::vector<int> binary) { // forms NxN image
  sf::Image result;
  int N{64};
  result.create(N, N);
  // controlla che binary.size() abbia senso

  int i{0};
  for(int y{0}; y < N; ++y) {
  for(int x{0}; x < N; ++x) {
    if (binary[i] == -1) {
      result.setPixel(x, y, sf::Color::Black);
    } else {
      result.setPixel(x, y, sf::Color::White);
    }
    ++i;
  }
  }

  return result;
}

} // namespace img

#endif