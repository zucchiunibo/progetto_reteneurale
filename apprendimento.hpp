#ifndef LEARN_HPP
#define LEARN_HPP

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace hope {

// Hebb's rule
std::vector<std::vector<double>>
computeMatrix(const std::vector<std::vector<int>>& patterns) {
  auto N{patterns[0].size()}; // number of neurons (pixels) per image
  auto P{patterns.size()};    // pattern number

  for (const auto& pattern : patterns) {
    if (pattern.size() != N)
      throw std::invalid_argument(
          "Tutti i pattern devono avere la stessa dimensione.");
  }

  std::vector<std::vector<double>> W(N, std::vector<double>(N, 0.0));

  for (unsigned int i{0}; i < static_cast<unsigned int>(N); ++i) {
    for (unsigned int j{0}; j < static_cast<unsigned int>(N); ++j) {
      if (i != j) {
        double muSum{0.0}; // sum up to P
        for (unsigned int mu{0}; mu < static_cast<unsigned int>(P); ++mu) {
          muSum += static_cast<double>(patterns[mu][i] * patterns[mu][j]);
        }

        W[i][j] = muSum / static_cast<double>(N);
      }
    }
  }

  return W;
}

void saveMatrix(const std::vector<std::vector<double>>& W,
                const std::string& filename) {
  auto N{W.size()};
  std::ofstream out(filename);

  if (!out)
    throw std::runtime_error(
        "Errore: impossibile aprire file per il salvataggio della matrice.");

  for (unsigned int i{0}; i < static_cast<unsigned int>(N); ++i) {
    for (unsigned int j{0}; j < static_cast<unsigned int>(N); ++j) {
      out << W[i][j];

      if (j + 1 < N)
        out << ' ';
    }
    out << '\n';
  }
}

void clearDirectory(const std::string& path) {
  namespace fs = std::filesystem;

  if (!fs::exists(path)) {
    std::cerr << "Cartella '" << path << "' non trovata.\n";
  }

  for (const auto& entry : fs::directory_iterator(path)) {
    if (fs::is_regular_file(entry)) {
      fs::remove(entry);
    }
  }
}

} // namespace hope

#endif