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

namespace rcl {

// Calcolo dell'energia della rete (Lyapunov function)
double energy(const std::vector<int>& state,
              const std::vector<std::vector<double>>& W) {
  const int N = state.size();
  double E    = 0.0;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      E += W[i][j] * state[i] * state[j];
    }
  }
  return -0.5 * E;
}

// Funzione di recall della rete di Hopfield
std::vector<int> recall(std::vector<int> corrupted,
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
    sf::Image recallImage = img::formImage(newVector);

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

} // namespace rcl

#endif