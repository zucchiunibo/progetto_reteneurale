#ifndef LEARN_HPP
#define LEARN_HPP

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace learn {

std::vector<std::vector<double>>

computeWeightMatrix(const std::vector<std::vector<int>>& patterns) {
  unsigned int N = patterns[0].size();
  unsigned int P = patterns.size();

  std::vector<std::vector<double>> W(N, std::vector<double>(N, 0.0));

  for (unsigned int i = 0; i < N; ++i) {
    for (unsigned int j = 0; j < N; ++j) {
      if (i != j) {
        double sum_mu = 0.0;
        for (unsigned int mu = 0; mu < P; ++mu) {
          sum_mu += static_cast<double>(patterns[mu][i] * patterns[mu][j]);
        }

        W[i][j] = sum_mu / static_cast<double>(N);
      }
    }
  }

  return W;
}

void saveWeightMatrix(const std::vector<std::vector<double>>& W, const std::string& filename) {

  unsigned int N = W.size();

  std::ofstream out(filename);

  if (!out) throw std::runtime_error("Errore: impossibile aprire file per il salvataggio della matrice.");

  for (unsigned int ni = 0; ni < N; ++ni) {
    for (unsigned int nj = 0; nj < N; ++nj) {
        out << W[ni][nj];

        if (nj+1<N) out << ' ';
      
    }
    out << '\n';
      
  }
  
}

} // namespace learn

#endif