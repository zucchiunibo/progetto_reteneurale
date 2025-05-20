#include "acquisition.hpp"
#include <iostream>
#include <vector>

int main() {
  sf::Image inputImage;
  if (!inputImage.loadFromFile("./patterns/original/ledzeppelin.jpg"))
    return -1;

  sf::Image resizedImage{img::resize(inputImage)};
  auto binaryVector{img::binaryConvert(resizedImage)};

  // for (int i{0}; i < binaryVector.size(); ++i) {
  //   std::cout << binaryVector[i] << ' ';
  // }
  // std::cout << '\n';
  sf::Image binary(img::formImage(binaryVector));

  if (!binary.saveToFile("./patterns/binary/result.png"))
    return -1;
}