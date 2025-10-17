#ifndef BALL_H 
#define BALL_H

#include <SFML/Graphics.hpp>

class Ball {
public:
    Ball(float startX, float startY);

    void update();  // move a bola
    void reset(float startX, float startY, float speedDirection); // reposiciona no centro
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const; // para colisão externa
    sf::Vector2f getPosition() const;

    void bounceX(); // inverte direção no eixo X
    void bounceY(); // inverte direção no eixo Y

private:
    sf::CircleShape shape;
    sf::Vector2f velocity; // velocidade em x e y
};

#endif
