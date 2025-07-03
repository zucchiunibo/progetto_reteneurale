#include "acquisition.hpp"
#include "apprendimento.hpp"
#include "recall.hpp"
#include <iostream>

int main() {
  std::vector<std::string> patternNames = {"kirby.png", "exuvia.jpg",
                                           "ledzeppelin.jpg", "orecchino.png"};
  std::vector<std::vector<int>> patterns;

  // FASE DI APPRENDIMENTO
  for (unsigned int i{0}; i < patternNames.size(); ++i) {
    sf::Image inputImage;
    if (!inputImage.loadFromFile("./patterns/original/" + patternNames[i])) {
      std::cerr << "Errore nel caricamento di: " << patternNames[i] << '\n';
      return -1;
    }

    auto resizedImage{hope::resize(inputImage)};
    auto binaryVector{hope::binaryConvert(resizedImage)};

    if (static_cast<int>(binaryVector.size()) != 64 * 64) {
      std::cerr << "Errore: dimensione pattern non valida per "
                << patternNames[i] << '\n';
      return -1;
    }

    auto binary(hope::formImage(binaryVector));

    if (!resizedImage.saveToFile("./patterns/resized/" + patternNames[i])) {
      std::cerr << "Errore nel salvataggio immagine ridimensionata: "
                << patternNames[i] << '\n';
      return -1;
    }

    if (!binary.saveToFile("./patterns/binary/" + patternNames[i])) {
      std::cerr << "Errore nel salvataggio immagine binaria: "
                << patternNames[i] << '\n';
      return -1;
    }

    patterns.push_back(binaryVector);
  }

  auto W = learn::computeWeightMatrix(patterns); // creare la matrice W
  learn::saveWeightMatrix(
      W, "./WeightsMatrix.txt"); // stampare su file testo la matrice

  std::cout << "Fase di apprendimento completata e matrice salvata.\n";

  // FASE DI CORRUZIONE
  sf::Image subject;
  if (!subject.loadFromFile("./patterns/original/exuvia.jpg")) {
    std::cerr << "Errore nel caricamento dell'immagine orecchino.png\n";
    return -1;
  }

  sf::Image resizedSubject{hope::resize(subject)};
  auto binarySubject{hope::binaryConvert(resizedSubject)};
  auto corruptedSubject{hope::corruption(binarySubject)};
  sf::Image binaryC(hope::formImage(corruptedSubject));
  if (!binaryC.saveToFile("./patterns/corrupted/corrupted.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine corrotta\n";
    return -1;
  }

  // FASE DI RICHIAMO (RECALL)
  auto recallVector{rcl::recall(corruptedSubject, W)};
  sf::Image binaryR(hope::formImage(recallVector));

  if (!binaryR.saveToFile("./patterns/binary/resultRecallFinal.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine finale richiamata\n";
    return -1;
  }

  std::cout << "Fase di richiamo completata e immagine finale salvata.\n";
  return 0;
}