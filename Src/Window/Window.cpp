#include "Window.hpp"
#include <algorithm>
void Window::init(){
    window.create(sf::VideoMode({1000,1000}),"Universe",sf::Style::Titlebar | sf::Style::Close);
    fullscreen=false;
    Camera=sf::View({0,0},{1000,1000});
    window.setFramerateLimit(60);
    DtClock.start();
}
#include <iostream>
void Window::HandleEvent(){
    dt=DtClock.getElapsedTime().asSeconds();
    DtClock.restart();
    window.setView(Camera);
    while (const std::optional Event = window.pollEvent())
    {
        if (Event->is<sf::Event::Closed>()){
            window.close();
        }
        if (const auto* wheel = Event->getIf<sf::Event::MouseWheelScrolled>())
        {
            scroll -= wheel->delta*dt;
            scroll=std::clamp(scroll,0.1f,3.f);
        }
        if (const auto* key = Event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::F11)
            {
                fullscreen = !fullscreen;

                if (fullscreen)
                {

                    window.create(sf::VideoMode::getDesktopMode(),
                                "Universe",
                                sf::Style::None,
                                sf::State::Fullscreen);
                }
                else
                {
                    window.create(sf::VideoMode({1000,1000}),
                                "Universe",
                                sf::Style::Titlebar | sf::Style::Close);
                }

                dt = 0;
                DtClock.restart();
                window.setFramerateLimit(60);

                float windowRatio = (float)window.getSize().x / window.getSize().y;
                float viewRatio = 1.f;

                sf::FloatRect viewport;

                if (windowRatio > viewRatio)
                {
                    viewport.size.x = viewRatio / windowRatio;
                    viewport.size.y = 1.f;
                    viewport.position.x = (1.f - viewport.size.x) / 2.f;
                    viewport.position.y = 0.f;
                }
                else
                {
                    viewport.size.x = 1.f;
                    viewport.size.y = windowRatio / viewRatio;
                    viewport.position.x = 0.f;
                    viewport.position.y = (1.f - viewport.size.y) / 2.f;
                }

                Camera.setViewport(viewport);
            }
        }
    }
    
}

void Window::Movement(){
    float speed=128*scroll;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
        Camera.move(sf::Vector2f(0,-speed)*dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
        Camera.move(sf::Vector2f(0,speed)*dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
        Camera.move(sf::Vector2f(speed,0)*dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
        Camera.move(sf::Vector2f(-speed,0)*dt);
    }
    Camera.setSize({1000*scroll,1000*scroll});
}