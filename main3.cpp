#include "acquisition.hpp"
#include "apprendimento.hpp"
#include "corruption.hpp"
#include "recall.hpp"
#include <iostream>
#include <vector>

int main() {
  std::vector<std::string> patternNames = {"kirby.png", "exuvia.jpg",
                                           "ledzeppelin.jpg", "orecchino.png"};
  std::vector<std::vector<int>> patterns;

  // FASE DI APPRENDIMENTO
  for (const auto& name : patternNames) {
    sf::Image inputImage;
    if (!inputImage.loadFromFile("./patterns/original/" + name)) {
      std::cerr << "Errore nel caricamento di: " << name << '\n';
      return -1;
    }

    sf::Image resizedImage{img::resize(inputImage)};
    auto binaryVector{img::binaryConvert(resizedImage)};

    if (binaryVector.size() != 64 * 64) {
      std::cerr << "Errore: dimensione pattern non valida per " << name << '\n';
      return -1;
    }

    sf::Image binary(img::formImage(binaryVector));

    if (!resizedImage.saveToFile("./patterns/resized/" + name)) {
      std::cerr << "Errore nel salvataggio immagine ridimensionata: " << name
                << '\n';
      return -1;
    }

    if (!binary.saveToFile("./patterns/binary/" + name)) {
      std::cerr << "Errore nel salvataggio immagine binaria: " << name << '\n';
      return -1;
    }

    patterns.push_back(binaryVector);
  }

  auto W = lrn::computeWeightMatrix(patterns); // creare la matrice W
  lrn::saveWeightMatrix(
      W, "./WeightsMatrix.txt"); // stampare su file testo la matrice

  std::cout << "Fase di apprendimento completata e matrice salvata.\n";

  // FASE DI CORRUZIONE
  sf::Image inputImage2;
  if (!inputImage2.loadFromFile("./patterns/original/orecchino.png")) {
    std::cerr << "Errore nel caricamento dell'immagine orecchino.png\n";
    return -1;
  }

  sf::Image resizedImage2{img::resize(inputImage2)};
  auto binaryVector2{img::binaryConvert(resizedImage2)};
  auto corruptedVector{crp::corruption(binaryVector2)};
  // for (int i{0}; i < binaryVector.size(); ++i) {
  //   std::cout << binaryVector[i] << ' ';
  // }
  // std::cout << '\n';
  sf::Image binaryC(img::formImage(corruptedVector));
  if (!binaryC.saveToFile("./patterns/binary/resultC.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine corrotta\n";
    return -1;
  }

  // FASE DI RICHIAMO (RECALL)
  auto recallVector{rcl::recall(corruptedVector, W)};
  sf::Image binaryR(img::formImage(recallVector));

  if (!binaryR.saveToFile("./patterns/binary/resultRecallFinal.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine finale richiamata\n";
    return -1;
  }

  std::cout << "Fase di richiamo completata e immagine finale salvata.\n";
  return 0;
}