#pragma once
#include <SFML/Graphics.hpp>
struct Window
{
    sf::RenderWindow window;
    float dt=0;
    sf::View Camera;
    float scroll=1;
    
    void init();
    void HandleEvent();
    void Movement();

    private:
    bool fullscreen;
    sf::Clock DtClock;
};
