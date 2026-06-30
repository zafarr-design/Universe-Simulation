#pragma once
#include "../Window/Window.hpp"
#include "../World/World.hpp"
struct Engine
{
    bool init=false;
    Window window;
    World world;

    void Run();
};
