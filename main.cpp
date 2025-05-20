#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./orecchino.png"))
    return -1;

  auto pixels{img::getPixels(inputImage)};

  sf::Image resizedImage{img::resize(inputImage)};
  if (!resizedImage.saveToFile("./result.png"))
    return -1;
}