#include "acquisition.hpp"
#include "learning.hpp"
#include "recall.hpp"
#include <iostream>

int main() {
  hope::clearDirectory("./matrix");
  hope::clearDirectory("./patterns/recall");
  std::vector<std::string> patternNames = {"kirby.png", "ledzeppelin.jpg", "exuvia.jpg", "A.jpg", "spaceinv.jpg", "yinyang.png", "orecchino.png"};
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
      std::cerr << "Errore: dimensione pattern non valida per " << patternNames[i] << '\n';
      return -1;
    }

    auto binary(hope::formImage(binaryVector));

    if (!resizedImage.saveToFile("./patterns/resized/" + patternNames[i])) {
      std::cerr << "Errore nel salvataggio immagine ridimensionata: " << patternNames[i] << '\n';
      return -1;
    }

    if (!binary.saveToFile("./patterns/binary/" + patternNames[i])) {
      std::cerr << "Errore nel salvataggio immagine binaria: " << patternNames[i] << '\n';
      return -1;
    }

    patterns.push_back(binaryVector);
  }

  // auto W = hope::computeMatrix(patterns);
  // hope::saveMatrix(W, "./matrix/WeightMatrix.txt"); // stampare su file testo la matrice

  std::cout << "Fase di apprendimento completata e matrice salvata.\n";

  // FASE DI CORRUZIONE
  sf::Image subject;
  if (!subject.loadFromFile("./patterns/original/spaceinv.jpg")) {
    std::cerr << "Errore nel caricamento dell'immagine\n";
    return -1;
  }

  auto corruptedSubject{hope::directCorruption(subject)};
  sf::Image binaryC(hope::formImage(corruptedSubject));
  if (!binaryC.saveToFile("./patterns/corrupted/corrupted.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine corrotta\n";
    return -1;
  }

  // FASE DI RICHIAMO (RECALL)

  // Classic Hopfield
   auto recallVector{hope::recallDAM(corruptedSubject, patterns)};

  // Modern Hopfield
  // auto recallVector{hope::recallDAM(corruptedSubject, patterns)};



  sf::Image binaryR(hope::formImage(recallVector));

  if (!binaryR.saveToFile("./patterns/recall/resultRecallFinal.png")) {
    std::cerr << "Errore nel salvataggio dell'immagine finale richiamata\n";
    return -1;
  }

  std::cout << "Fase di richiamo completata e immagine finale salvata.\n";

  return 0;
}