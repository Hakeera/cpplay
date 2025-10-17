#ifndef PADDLE_H
#define PADDLE_H

#include <SFML/Graphics.hpp>

class Paddle {
public:
    Paddle(float x, float y);
    void moveUp();
    void moveDown();
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const; // para colisão externa

private:
    sf::RectangleShape shape;
    float speed;
};

#endif
