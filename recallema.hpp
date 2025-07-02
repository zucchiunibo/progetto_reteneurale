#ifndef RECALL_HPP
#define RECALL_HPP
#include "acquisition.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
namespace rcl {

auto recall(std::vector<int> corrupted, std::vector<std::vector<double>> W) {
  bool i = true;

  sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");
  sf::Texture texture;
  while (i & window.isOpen()) {
    std::vector<int> newVector;
    int j = 0;
    for (int n{0}; n <= (64 * 64); n++) {
      int newValue;
      double sum{std::accumulate(
          W[n][0] * corrupted[0], W[n][64 * 64] * corrupted[64 * 64], 0.,
          [](double acc, std::vector<std::vector<double>> const& W,
             std::vector<int> const& corrupted, int n,
             int m) { return acc + (W[n][m + 1] * corrupted[m + 1]); })};
      double energy(){
        int sumE{0};
        for (int i{0};i<=64*64; i++ ){
          for (int j{0}; j<=64, j++){
            sumE+=(W[i][j]*corrupted[i]*corrupted[j]);
          }
        }
        return -(1/2)*sumE;
      }
      std::cout(energy)
      int newValue = abs(sum) / sum;
      newVector.push_back(newValue);

      if (newVector[n] == corrupted[n]) {
        j += 1;
      }
    }
    sf::Image RecallImage(img::formImage(newVector));
    if (!RecallImage.saveToFile("./patterns/binary/resultRecall.png"))

      if (!texture.loadFromFile("./patterns/binary/resultRecall.png")) {
      }
    sf::Sprite image;
    image.setTexture(texture);

    window.clear(sf::Color::Black);
    window.draw(image);
    if (j == (64 * 64)) {
      i = false;
      return newVector;
    } else {
      corrupted = newVector;
    }
  }
}

} // namespace rcl

#endif