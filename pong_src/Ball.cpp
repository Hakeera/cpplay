#include "Ball.hpp"
#include "Config.hpp"

Ball::Ball(float startX, float startY) {
    shape.setRadius(BALL_RADIUS);
    shape.setFillColor(sf::Color::White);
    shape.setPosition(startX, startY);

    // Velocidade inicial
    velocity = sf::Vector2f(BALL_SPEED, BALL_SPEED);
}

void Ball::update() {
    shape.move(velocity);
}

void Ball::reset(float startX, float startY, float speedDirection) {
    shape.setPosition(startX, startY);
    velocity = sf::Vector2f(speedDirection, speedDirection);
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Ball::getBounds() const {
    return shape.getGlobalBounds();
}

sf::Vector2f Ball::getPosition() const {
    return shape.getPosition();
}

void Ball::bounceX() {
    velocity.x *= -1;
}

void Ball::bounceY() {
    velocity.y *= -1;
}
