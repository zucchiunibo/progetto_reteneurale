#include <SFML/Graphics/Image.hpp>

int main() {
  sf::Image orecchino;
  if (!orecchino.loadFromFile("orecchino.png"))
    return -1;

  sf::Color color = orecchino.getPixel(0, 0);
  color.a         = 0;
  orecchino.setPixel(0, 0, color);
  if (!orecchino.saveToFile("result.png"))
    return -1;
}