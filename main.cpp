#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./patterns/original/kirby.png"))
    return -1;

  sf::Image resizedImage{img::resize(inputImage)};
  auto binaryVector{img::binaryConvert(resizedImage)};

  sf::Image binary(img::formImage(binaryVector));

  if (!resizedImage.saveToFile("./patterns/resized/kirby.png"))
    return -1;
  
  if (!binary.saveToFile("./patterns/binary/kirby.png"))
    return -1;
}