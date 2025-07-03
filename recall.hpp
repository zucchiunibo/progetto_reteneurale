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

// Calcolo dell'energia della rete (Lyapunov function)
double energy(const std::vector<int>& state,
              const std::vector<std::vector<double>>& W) {
  if (W.size() != state.size())
    throw std::invalid_argument(
        "La matrice W e lo stato hanno dimensioni incompatibili.");

  const auto N{static_cast<unsigned int>(state.size())};
  double E{0.0};
  for (unsigned int i{0}; i < N; ++i) {
    for (unsigned int j{0}; j < N; ++j) {
      E += static_cast<double>(W[i][j] * state[i] * state[j]);
    }
  }
  return -0.5 * E;
}

std::vector<int> recall(std::vector<int> corrupted,
                        const std::vector<std::vector<double>>& W) {
  const auto N{static_cast<unsigned int>(corrupted.size())};

  sf::RenderWindow window(sf::VideoMode(800, 600), "Hopfield Recall");
  sf::Texture texture;

  double prevEnergy = energy(corrupted, W);
  std::cout << "Energia iniziale: " << prevEnergy << '\n';

  int unchanged{0};
  int t{0};

  while (window.isOpen()) {
    unchanged = 0;
    std::vector<int> newVector(N);

    for (unsigned int n{0}; n < N; ++n) {
      double sum{0.0};
      for (unsigned int m{0}; m < N; ++m) {
        sum += static_cast<double>(W[n][m] * corrupted[m]);
      }

      newVector[n] = (sum >= 0) ? 1 : -1;
      if (newVector[n] == corrupted[n])
        ++unchanged;
    }

    double current_energy = energy(newVector, W);
    std::cout << "Energia attuale: " << current_energy << '\n';

    // RIGUARDA MEGLIO
    if (current_energy > prevEnergy + 1e-6) {
      std::cerr << "Errore: energia aumentata! La rete non sta convergendo.\n";
      break;
    }

    prevEnergy = current_energy;

    // Salvataggio e visualizzazione dell'immagine attuale
    sf::Image recallImage = hope::formImage(newVector);

    if (!recallImage.saveToFile("./patterns/recall/resultRecall"
                                + std::to_string(t) + ".png")) {
      std::cerr << "Errore: impossibile salvare l'immagine resultRecall.png\n";
      break;
    }

    if (!texture.loadFromFile("./patterns/recall/resultRecall.png")) {
      std::cerr << "Errore: impossibile caricare la texture resultRecall.png\n";
      break;
    }

    sf::Sprite image(texture);

    // Controllo di convergenza
    if (unchanged == N) {
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
    ++t;
  }

  return corrupted;
}


std::vector<int> recallIA(std::vector<int> corrupted,
                        const std::vector<std::vector<double>>& W) {
  const unsigned int N = corrupted.size();
  std::vector<int> state = corrupted;

  std::random_device rd;
  std::mt19937 g(rd());

  double prevEnergy = energy(state, W);
  std::cout << "Energia iniziale: " << prevEnergy << '\n';

  int maxIterations = 100;
  int t = 0;

  while (t < maxIterations) {
    // Crea ordine casuale per aggiornamento asincrono
    std::vector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), g);

    bool anyChange = false;

    for (int i : indices) {
      double sum = 0.0;
      for (unsigned int j = 0; j < N; ++j)
        sum += W[i][j] * state[j];

      int newValue = (sum >= 0) ? 1 : -1;
      if (state[i] != newValue) {
        state[i] = newValue;
        anyChange = true;
      }
    }

    // Calcolo energia
    double currentEnergy = energy(state, W);
    std::cout << "Iterazione " << t << ", Energia: " << currentEnergy << '\n';

    // Salva l'immagine a ogni step
    sf::Image recallImage = hope::formImage(state);
    if (!recallImage.saveToFile("./patterns/recall/resultRecall" + std::to_string(t) + ".png")) {
      std::cerr << "Errore nel salvataggio dell'immagine resultRecall\n";
    }

    // Condizione di stop
    if (!anyChange || std::abs(currentEnergy - prevEnergy) < 1e-6) {
      std::cout << "Convergenza raggiunta dopo " << t << " iterazioni.\n";
      break;
    }

    prevEnergy = currentEnergy;
    ++t;
  }

  return state;
}


// Funzione di recall della rete di Hopfield
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

} // namespace hope

#endif