#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include <vector>

int main() {
  sf::Image orecchino;
  if (!orecchino.loadFromFile("./orecchino.png"))
    return -1;
  auto pixelArray{orecchino.getPixelsPtr()};
  auto size = orecchino.getSize();
  auto n    = size.x * size.y * 4;
  std::vector<sf::Color> pixels{};
  for (int i{0}; i < n / 4; i += 4) {
    pixels.push_back({pixelArray[i], pixelArray[i + 1], pixelArray[i + 2],
                      pixelArray[i + 3]});
  }

  for (int i{0}; i < pixels.size() / 4; i += 4) {
    pixels[i]     = sf::Color::Yellow;
    pixels[i + 1] = sf::Color::Red;
    pixels[i + 2] = sf::Color::Cyan;
    pixels[i + 3] = sf::Color::White;
  }

  for (int i{0}; i < size.x; ++i) {
    for (int j{0}; j < size.y; ++j) {
      orecchino.setPixel(i, j, pixels[i]);
    }
  }

  if (!orecchino.saveToFile("./result.png"))
    return -1;
}