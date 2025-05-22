#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./patterns/original/ledzeppelin.jpg"))
    return -1;

  sf::Image resizedImage{img::resize(inputImage)};
  auto binaryVector{img::binaryConvert(resizedImage)};

  sf::Image binary(img::formImage(binaryVector));

  if (!resizedImage.saveToFile("./patterns/resized/ledzeppelin.png"))
    return -1;
  
  if (!binary.saveToFile("./patterns/binary/ledzeppelin.png"))
    return -1;
}