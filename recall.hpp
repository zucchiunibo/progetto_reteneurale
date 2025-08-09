#ifndef RECALL_HPP
#define RECALL_HPP

#include "acquisition.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <ranges>
#include <string>
#include <vector>

namespace hope {

// Calcolo dell'energia Lyapunov della rete
double energy(const std::vector<int>& state, const std::vector<std::vector<double>>& W) {
  if (W.size() != state.size())
    throw std::invalid_argument("La matrice W e lo stato hanno dimensioni incompatibili.");

  const auto N{static_cast<unsigned int>(state.size())};
  double E{0.0};
  for (unsigned int i{0}; i < N; ++i) {
    for (unsigned int j{0}; j < N; ++j) {
      E += static_cast<double>(W[i][j] * state[i] * state[j]);
    }
  }
  return -0.5 * E;
}

// Funzione di recall della rete di Hopfield classica (MANCANO ERRORI E ACCUMULATE)
std::vector<int> recall(std::vector<int> state, const std::vector<std::vector<double>>& W) {
  const auto N{static_cast<unsigned int>(state.size())}; // numero di neuroni

  double prevEnergy = energy(state, W);
  std::cout << "Energia iniziale: " << prevEnergy << '\n';

  int unchanged{0};
  int t{0};
  bool converged{false};

  while (!converged) {
    unchanged = 0;
    std::vector<int> newVector(N);

    for (unsigned int n{0}; n < N; ++n) {
      double sum{0.0};
      for (size_t m = 0; m < N; ++m) {
        sum += W[n][m] * state[m];
      }

      newVector[n] = (sum >= 0.0) ? 1 : -1;
      if (newVector[n] == state[n])
        ++unchanged;
    }

    double currentEnergy = energy(newVector, W);
    std::cout << "Iterazione " << t << ", Energia: " << currentEnergy << '\n';

    prevEnergy = currentEnergy;

    // Controllo di convergenza
    if (unchanged == N) {
      std::cout << "La rete ha raggiunto uno stato stabile.\n";
      converged             = true;
      sf::Image recallImage = hope::formImage(newVector);
      if (!recallImage.saveToFile("./patterns/recall/resultRecallFinal.png")) {
        std::cerr << "Errore nel salvataggio dell'immagine al passo finale\n";
      }
    } else {
      sf::Image recallImage = hope::formImage(newVector);
      if (!recallImage.saveToFile("./patterns/recall/resultRecall" + std::to_string(t) + ".png")) {
        std::cerr << "Errore nel salvataggio dell'immagine al passo " << t << '\n';
      }
    }

    state = newVector;
    ++t;
  }

  return state;
}

// Funzione di recall della rete di Hopfield con Simulated Annealing (MANCANO ERRORI)
std::vector<int> simAnnealing(std::vector<int> state, const std::vector<std::vector<double>>& W) {
  const auto N{static_cast<unsigned int>(state.size())};
  int t{0}; // contatore di iterazioni

  double prevEnergy{energy(state, W)};
  std::cout << "Energia iniziale: " << prevEnergy << '\n';

  double T{2.0};      // Temperatura iniziale
  double Tmin{0.1};   // Temperatura minima finale
  double alpha{0.98}; // Fattore di raffreddamento (0 < alpha < 1)

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  bool converged{false};

  while (!converged) {
    for (unsigned int n{0}; n < N; ++n) {
      double sum{0.0};
      for (size_t m{0}; m < N; ++m) {
        sum += W[n][m] * state[m];
      }

      double deltaEnergy{2.0 * sum * state[n]}; // energia di flip di un neurone (ottenuta come differenza)

      double P{1.0 / (1.0 + std::exp(deltaEnergy / T))};

      double r{dis(gen)}; // numero casuale tra 0 e 1

      if (r < P) {
        state[n] = -state[n];
      }
    }

    T *= alpha;

    double currentEnergy = energy(state, W);
    std::cout << "Energia attuale: " << currentEnergy << '\n';
    std::cout << "T attuale: " << T << '\n';

    prevEnergy = currentEnergy;

    // Controllo di convergenza
    if (T <= Tmin) {
      std::cout << "La rete ha raggiunto uno stato stabile.\n";
      converged             = true;
      sf::Image recallImage = hope::formImage(state);
      if (!recallImage.saveToFile("./patterns/recall/resultRecallFinal.png")) {
        std::cerr << "Errore nel salvataggio dell'immagine al passo finale\n";
      }
    }
    // else {
    //   sf::Image recallImage = hope::formImage(state);
    //   if (!recallImage.saveToFile("./patterns/recall/resultRecall" + std::to_string(t) + ".png")) {
    //     std::cerr << "Errore nel salvataggio dell'immagine al passo " << t << '\n';
    //   }
    // }
    ++t;
  }
  return state;
}

// Funzione di recall della rete di Hopfield con DAM (DA RIVEDERE EXP)
std::vector<int> recallDAM(std::vector<int>& state, const std::vector<std::vector<int>>& patterns, bool pol) {
  const auto N = state.size();
  const auto P = patterns.size();

  auto F = [pol](double z) { return pol ? static_cast<double>(std::pow(z, 4.0)) : static_cast<double>(std::exp(z)); };

  for (size_t k{0}; k < N; ++k) {
    double dE{0.0};
    double enPlus{0.0};
    double enMinus{0.0};
    for (size_t mu{0}; mu < P; ++mu) {
      double sumPlus{0.0};
      double sumMinus{0.0};
      for (size_t i{0}; i < N; ++i) {
        sumPlus += (i == k) ? patterns[mu][i] : (patterns[mu][i] * state[i]);
        sumMinus += (i == k) ? -patterns[mu][i] : (patterns[mu][i] * state[i]);
      }
      enPlus -= F(sumPlus);
      enMinus -= F(sumMinus);
    }
    dE       = enMinus - enPlus;
    state[k] = (dE >= 0) ? 1 : -1;
  }

  return state;
}

} // namespace hope

#endif