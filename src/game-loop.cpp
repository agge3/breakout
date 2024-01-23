#include "game_loop.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

constexpr int window_width{800}, window_height{600};

void Game::run()
{
    // main game loop
}

void Game::update()
{
}

void Game::run()
{
    // creation of game window
    RenderWindow window{{window_width, window_height}, "Practice Game"};
    window.setFramerateLimit(60);

    // main game loop
    while (window.isOpen()) {
        // clear the window from previously drawn graphics
        window.clear(Color::Black);

        // if "escape" is pressed, break out of the loop
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
            break;

        // show the window contents
        window.display();
    }

    return 0;
}
