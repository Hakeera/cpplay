#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "Ball.hpp"
#include "Config.hpp"
#include "Paddle.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong");

    // Bola
    Ball ball(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

    // Raquetes
    Paddle paddleLeft(50, WINDOW_HEIGHT/2 - PADDLE_HEIGHT/2);
    Paddle paddleRight(WINDOW_WIDTH - 50 - PADDLE_WIDTH, WINDOW_HEIGHT/2 - PADDLE_HEIGHT/2);
    
    // Placar
    int scoreLeft = 0;
    int scoreRight = 0;

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Erro ao carregar fonte\n";
        return -1;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(360, 10);

    // Loop principal
    while (window.isOpen()) {
        sf::Event event;
       while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Movimento das raquetes
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) 
            paddleLeft.moveUp();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) 
            paddleLeft.moveDown();

        // Movimento das raquetes
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
            paddleRight.moveUp();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
            paddleRight.moveDown();

        // Movimento da bola
	ball.update();

        // Colisão com bordas superior/inferior
        if (ball.getPosition().y <= 0 || ball.getPosition().y + BALL_RADIUS*2 >= WINDOW_HEIGHT){
		ball.bounceY();
	}

        // Colisão com raquetes
        if (ball.getBounds().intersects(paddleLeft.getBounds()) ||
            ball.getBounds().intersects(paddleRight.getBounds())) {
            ball.bounceX();
        }

        // Pontuação
        if (ball.getPosition().x <= 0) {
            scoreRight++;
	    ball.reset(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, BALL_SPEED);
        }
        if (ball.getPosition().x + 20 >= WINDOW_WIDTH) {
            scoreLeft++;
	    ball.reset(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, -BALL_SPEED);
        }

        // Atualizar texto do placar
        scoreText.setString(std::to_string(scoreLeft) + " - " + std::to_string(scoreRight));

        // Renderização
        window.clear();
	paddleLeft.draw(window);
	paddleRight.draw(window);
	ball.draw(window);
        window.draw(scoreText);
        window.display();
    }

    return 0;
}
