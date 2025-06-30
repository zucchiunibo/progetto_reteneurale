#include "acquisition.hpp"
#include "apprendimento.hpp"
#include <iostream>
#include <vector>

int main() {
  
  std::vector<std::string> patternNames = {"kirby.png", "exuvia.jpg",
                                           "ledzeppelin.jpg", "orecchino.png"};
  std::vector<std::vector<int>> patterns;

  for (const auto& name : patternNames) {
    sf::Image inputImage;
    if (!inputImage.loadFromFile("./patterns/original/" + name)) {
      return -1;
    }

    sf::Image resizedImage{img::resize(inputImage)};
    auto binaryVector{img::binaryConvert(resizedImage)};

    if (binaryVector.size() != 64 * 64) {
      return -1;
    }

    sf::Image binary(img::formImage(binaryVector));

    if (!resizedImage.saveToFile("./patterns/resized/" + name)) {
      return -1;
    }

    if (!binary.saveToFile("./patterns/binary/" + name)) {
      return -1;
    }

    patterns.push_back(binaryVector);
  }

  auto W = learn::computeWeightMatrix(patterns); // creare la matrice W

  learn::saveWeightMatrix(W, "./WeightsMatrix.txt"); // stampare su file testo la matrice

  std::cout << "Fase di apprendimento completata e matrice salvata.\n";

  
}