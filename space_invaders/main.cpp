#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

// ==================== Utilitários ====================

void setNonBlockingInput(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void clearScreen() {
    std::cout << "\033[2J\033[H";
}

// ==================== Estruturas ====================

const int WIDTH = 80;
const int HEIGHT = 20;

struct Player {
    int x;
    char symbol = '@';
};

struct Bullet {
    int x, y;
    char symbol = '$';
};

struct Enemy {
    int x, y;
    bool alive = true;
    char symbol = 'W';
};

// ==================== Funções auxiliares ====================

void drawBorder(std::vector<std::string> &screen) {
    for (int x = 0; x < WIDTH + 2; x++) {
        screen[0][x] = '#';
        screen[HEIGHT + 1][x] = '#';
    }
    for (int y = 0; y < HEIGHT + 2; y++) {
        screen[y][0] = '#';
        screen[y][WIDTH + 1] = '#';
    }
}

// ==================== Código principal ====================

int main() {
    Player player{WIDTH / 2};
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    // Formação inicial de inimigos
    int rows = 3, cols = 8;
    int startX = 5, startY = 2, spacing = 4;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            enemies.push_back(Enemy{startX + c * spacing, startY + r * 2});
        }
    }

    bool running = true;
    setNonBlockingInput(true);

    auto lastFrame = std::chrono::steady_clock::now();
    int direction = 1;
    int frameCounter = 0;
    int score = 0;

    while (running) {
        // ===== Entrada =====
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'a' || ch == 'A') player.x = std::max(0, player.x - 1);
            if (ch == 'd' || ch == 'D') player.x = std::min(WIDTH - 1, player.x + 1);
            if (ch == ' ') bullets.push_back(Bullet{player.x, HEIGHT - 2});
            if (ch == 'q' || ch == 'Q') running = false;
        }

        // ===== Atualização =====
        for (auto &b : bullets) b.y -= 1;
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                           [](const Bullet &b) { return b.y < 0; }),
            bullets.end());

        frameCounter++;
        if (frameCounter % 10 == 0) {
            bool bounce = false;
            for (auto &e : enemies) {
                if (!e.alive) continue;
                if ((direction == 1 && e.x >= WIDTH - 2) ||
                    (direction == -1 && e.x <= 1)) {
                    bounce = true;
                    break;
                }
            }

            if (bounce) {
                direction *= -1;
                for (auto &e : enemies)
                    if (e.alive) e.y += 1;
            } else {
                for (auto &e : enemies)
                    if (e.alive) e.x += direction;
            }
        }

        // Colisões
        for (auto &b : bullets) {
            for (auto &e : enemies) {
                if (e.alive && b.x == e.x && b.y == e.y) {
                    e.alive = false;
                    b.y = -1;
                    score += 10;
                }
            }
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                           [](const Bullet &b) { return b.y < 0; }),
            bullets.end());

        for (auto &e : enemies) {
            if (e.alive && e.y >= HEIGHT - 1) {
                clearScreen();
                std::cout << "\n\n   💀 GAME OVER 💀\n\n";
                std::cout << "   Pontuação final: " << score << "\n\n";
                setNonBlockingInput(false);
                return 0;
            }
        }

        // ===== Renderização =====
        clearScreen();

        std::vector<std::string> screen(HEIGHT + 2, std::string(WIDTH + 2, ' '));
        drawBorder(screen);

        // Player
        screen[HEIGHT][player.x + 1] = player.symbol;

        // Bullets
        for (auto &b : bullets)
            if (b.y >= 0 && b.y < HEIGHT)
                screen[b.y + 1][b.x + 1] = b.symbol;

        // Enemies
        for (auto &e : enemies)
            if (e.alive && e.y >= 0 && e.y < HEIGHT)
                screen[e.y + 1][e.x + 1] = e.symbol;

        // HUD
        for (auto &line : screen) std::cout << line << "\n";
        std::cout << "Score: " << score
                  << " | [A/D mover] [ESPACO atirar] [Q sair]\n";

        // ===== FPS =====
        auto now = std::chrono::steady_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrame);
        if (frameTime.count() < 33)
            std::this_thread::sleep_for(std::chrono::milliseconds(33 - frameTime.count()));
        lastFrame = now;
    }

    setNonBlockingInput(false);
    return 0;
}
