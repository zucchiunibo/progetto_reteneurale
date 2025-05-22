#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./patterns/original/orecchino.png"))
    return -1;

  sf::Image resizedImage{img::resize(inputImage)};
  auto binaryVector{img::binaryConvert(resizedImage)};

  sf::Image binary(img::formImage(binaryVector));
  
  if (!binary.saveToFile("./patterns/binary/result.png"))
    return -1;
}