#ifndef RECALL_HPP
#define RECALL_HPP

#include "acquisition.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <random>
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
  const auto N{static_cast<unsigned int>(state.size())};

  double prevEnergy = energy(state, W);
  std::cout << "Energia iniziale: " << prevEnergy << '\n';

  int unchanged{0};
  int t{0};
  bool converged{false};

  while (!converged) {
    unchanged = 0;
    std::vector<int> newVector(N);

    for (unsigned int n{0}; n < N; ++n) {
      double sum{
          std::accumulate(W[n].begin(), W[n].end(), 0.0, [&state, &W, n](double acc, size_t m = 0) {
            acc += W[n][m] * state[m];
            ++m;
            return acc;
          })};

      newVector[n] = (sum >= 0) ? 1 : -1;
      if (newVector[n] == state[n])
        ++unchanged;
    }

    double currentEnergy = energy(newVector, W);
    std::cout << "Iterazione " << t << ", Energia: " << currentEnergy << '\n';

    // RIGUARDA MEGLIO
    if (currentEnergy > prevEnergy + 1e-6) {
      std::cerr << "Errore: energia aumentata! La rete non sta convergendo.\n";
      break;
    }

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

// Funzione di recall della rete di Hopfield con Simulated Annealing (DA
// REVISIONARE)
std::vector<int> simAnnealing(std::vector<int> corrupted,
                              const std::vector<std::vector<double>>& W) {
  const int N = corrupted.size();
  sf::RenderWindow window(sf::VideoMode(800, 600), "Hopfield Recall");
  sf::Texture texture;

  double prev_energy = energy(corrupted, W);
  std::cout << "Energia iniziale: " << prev_energy << '\n';

  double T     = 1.0;  // Temperatura iniziale
  double Tmin  = 0.01; // Temperatura minima finale
  double alpha = 0.95; // Fattore di raffreddamento (0 < alpha < 1)

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  while (window.isOpen()) {
    std::vector<int> newVector(N);
    int unchanged = 0;

    for (int n = 0; n < N; ++n) {
      double sum = 0.0;

      for (int m = 0; m < N; ++m) {
        sum += W[n][m] * corrupted[m];
      }

      double delta_energy = 2.0 * sum * corrupted[n];

      double P = 1.0 / (1.0 + std::exp(delta_energy / T));

      double r = dis(gen);

      if (r < P) {
        // Cambio stato neurone
        newVector[n] = -corrupted[n];
      } else {
        newVector[n] = corrupted[n];
        ++unchanged;
      }
    }

    T *= alpha;
    if (T < 1e-3)
      T = 1e-3;

    double current_energy = energy(newVector, W);
    std::cout << "Energia attuale: " << current_energy << '\n';
    std::cout << "T attuale: " << T << '\n';

    prev_energy = current_energy;

    // Salvataggio e visualizzazione dell'immagine attuale
    sf::Image recallImage = hope::formImage(newVector);

    if (!recallImage.saveToFile("./patterns/binary/resultRecall.png")) {
      std::cerr << "Errore: impossibile salvare l'immagine resultRecall.png\n";
      break;
    }

    if (!texture.loadFromFile("./patterns/binary/resultRecall.png")) {
      std::cerr << "Errore: impossibile caricare la texture resultRecall.png\n";
      break;
    }

    sf::Sprite image(texture);

    // Controllo di convergenza
    if ((unchanged == N) || (T == Tmin)) {
      std::cout << "La rete ha raggiunto uno stato stabile.\n";

      // Tenere aperta la finestra finchè non viene chiusa

      sf::Event event;
      window.clear(sf::Color::Black);
      window.draw(image);
      window.display();

      while (window.waitEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
      }

      break;
    }

    corrupted = newVector;
  }

  return corrupted;
}

// Funzione di recall della rete di Hopfield con DAM (Dynamic Associative
// Memory) (DA REVISIONARE)
std::vector<int> recallDAM(std::vector<int> state, const std::vector<std::vector<int>>& patterns,
                           int n = 4 // esponente per F(z) = z^n
) {
  const size_t N            = state.size();
  const size_t P            = patterns.size();
  std::vector<int> newState = state;

  auto F = [&](double z) { return std::pow(z, n); };

  for (size_t l = 0; l < N; ++l) {
    double sumPlus = 0.0, sumMinus = 0.0;
    // valuta state con ξ[l]=+1 / -1
    for (size_t mu = 0; mu < P; ++mu) {
      double dotPlus = 0.0, dotMinus = 0.0;
      for (size_t k = 0; k < N; ++k) {
        int s = (k == l ? +1 : state[k]);
        dotPlus += patterns[mu][k] * s;
        s = (k == l ? -1 : state[k]);
        dotMinus += patterns[mu][k] * s;
      }
      sumPlus += F(dotPlus);
      sumMinus += F(dotMinus);
    }
    newState[l] = (sumPlus - sumMinus >= 0) ? +1 : -1;
  }

  return newState;
}

} // namespace hope

#endif