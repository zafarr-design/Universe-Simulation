#include "Engine.hpp"
#include <iostream>
void Engine::Run(){
    if (!init){
        window.init();
        world.init();
        init=true;
    }
    while (window.window.isOpen())
    {
        window.HandleEvent();
        if (window.window.hasFocus()){
            window.Movement();
        }
        window.window.clear();
        world.Step(window.dt,window.window,window.Camera,window.scroll);
        window.window.display();
    }
}