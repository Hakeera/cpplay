#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>

// =====================
// Constantes do jogo
// =====================
static const float WINDOW_W   = 800.f;
static const float WINDOW_H   = 600.f;
static const float GRAVITY    = 2000.f;   // px/s^2
static const float MOVE_SPEED = 300.f;    // px/s
static const float JUMP_SPEED = 700.f;    // px/s (para cima → negativo no eixo Y da SFML)

// =====================
// Entidades básicas
// =====================
struct Platform {
    sf::RectangleShape shape;
    Platform(const sf::Vector2f& size, const sf::Vector2f& pos, sf::Color color = sf::Color::White) {
        shape.setSize(size);
        shape.setFillColor(color);
        shape.setPosition(pos);
    }
};

struct Player {
    sf::RectangleShape shape;
    sf::Vector2f velocity {0.f, 0.f};
    bool onGround = false;

    Player(const sf::Vector2f& size, const sf::Vector2f& pos, sf::Color color = sf::Color::White) {
        shape.setSize(size);
        shape.setFillColor(color);
        shape.setPosition(pos);
    }
};

// =====================
// Utilidades
// =====================
static inline float half(float v) { return v * 0.5f; }

// Resolve colisão AABB (axis-aligned) entre um corpo móvel (player) e uma plataforma estática.
// Empurra o player para fora pelo menor eixo e zera a componente de velocidade resolvida.
void resolveCollision(Player& p, const Platform& pl) {
    sf::FloatRect a = p.shape.getGlobalBounds();
    sf::FloatRect b = pl.shape.getGlobalBounds();

    // Centros
    sf::Vector2f aCenter(a.left + half(a.width), a.top + half(a.height));
    sf::Vector2f bCenter(b.left + half(b.width), b.top + half(b.height));

    // Distâncias entre centros
    float deltaX = aCenter.x - bCenter.x;
    float deltaY = aCenter.y - bCenter.y;

    // Sobreposição em cada eixo
    float intersectX = (half(a.width) + half(b.width)) - std::abs(deltaX);
    float intersectY = (half(a.height) + half(b.height)) - std::abs(deltaY);

    // Se ambos positivos, há colisão
    if (intersectX > 0.f && intersectY > 0.f) {
        if (intersectX < intersectY) {
            // Resolver no X
            if (deltaX > 0.f) {
                // Player está à direita → empurra para a direita
                p.shape.move(intersectX, 0.f);
            } else {
                // Player está à esquerda → empurra para a esquerda
                p.shape.move(-intersectX, 0.f);
            }
            p.velocity.x = 0.f;
        } else {
            // Resolver no Y
            if (deltaY > 0.f) {
                // Player está abaixo da plataforma → empurra para baixo
                p.shape.move(0.f, intersectY);
                // Batida por baixo: impede atravessar e mantém queda
                if (p.velocity.y < 0.f) p.velocity.y = 0.f;
                p.onGround = false;
            } else {
                // Player está acima da plataforma → empurra para cima
                p.shape.move(0.f, -intersectY);
                // Aterrissou
                if (p.velocity.y > 0.f) p.velocity.y = 0.f;
                p.onGround = true;
            }
        }
    }
}



// Mantém o player dentro da janela; trata chão/teto como superfícies sólidas simples
void clampToWindow(Player& p) {
    sf::Vector2f pos = p.shape.getPosition();
    sf::Vector2f sz  = p.shape.getSize();

    // Limites horizontais
    if (pos.x < 0.f) { pos.x = 0.f; p.velocity.x = 0.f; }
    if (pos.x + sz.x > WINDOW_W) { pos.x = WINDOW_W - sz.x; p.velocity.x = 0.f; }

    // Limites verticais
    if (pos.y < 0.f) { pos.y = 0.f; p.velocity.y = 0.f; p.onGround = false; }
    if (pos.y + sz.y > WINDOW_H) { pos.y = WINDOW_H - sz.y; p.velocity.y = 0.f; p.onGround = true; }

    p.shape.setPosition(pos);
}

int main() {
    // Janela
    sf::RenderWindow window(sf::VideoMode((unsigned)WINDOW_W, (unsigned)WINDOW_H), "GAME");
    window.setFramerateLimit(120); // opcional: estabiliza a simulação

    // Relógio para delta time
    sf::Clock clock;

    // Player e plataforma(s)
    Player player({20.f, 20.f}, {250.f, 200.f}, sf::Color::Cyan);
    Platform floor1({50.f, 20.f}, {50.f, 300.f});
    Platform floor2({100.f, 20.f}, {200.f, 300.f});

    // Loop do jogo
    while (window.isOpen()) {
        // --- Eventos ---
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // --- Delta time ---
        float dt = clock.restart().asSeconds();
        if (dt > 0.033f) dt = 0.033f; // trava dt máximo para evitar "tunneling" em pausas

        // --- Input horizontal ---
        float move = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move += 1.f;
        player.velocity.x = move * MOVE_SPEED;

        // --- Pulo ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.onGround) {
            player.velocity.y = -JUMP_SPEED; // negativo sobe (origem no topo)
            player.onGround = false;
        }

        // --- Gravidade ---
        player.velocity.y += GRAVITY * dt;

        // --- Integração de movimento ---
        player.shape.move(player.velocity * dt);

        // --- Colisões ---
        player.onGround = false; // recalcularemos via colisões
        resolveCollision(player, floor1);
        resolveCollision(player, floor2);
        clampToWindow(player);

        // --- Render ---
        window.clear();
        window.draw(floor1.shape);
        window.draw(floor2.shape);
        window.draw(player.shape);
        window.display();
    }

    return 0;
}
