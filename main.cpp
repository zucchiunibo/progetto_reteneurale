#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./patterns/original/orecchino.png"))
    return -1;

  auto pixels{img::getPixels(inputImage)};

  sf::Image resizedImage{img::resize(inputImage)};
  if (!resizedImage.saveToFile("./patterns/resized/result.png"))
    return -1;
}