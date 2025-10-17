#include "Paddle.hpp"
#include "Config.hpp"

Paddle::Paddle(float x, float y) {
    shape.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::White);
    speed = PADDLE_SPEED;
}

void Paddle::moveUp() {
    if (shape.getPosition().y > 0)
        shape.move(0, -speed);
}

void Paddle::moveDown() {
    if (shape.getPosition().y + PADDLE_HEIGHT < WINDOW_HEIGHT)
        shape.move(0, speed);
}

void Paddle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Paddle::getBounds() const {
    return shape.getGlobalBounds();
}

