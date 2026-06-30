#pragma once
#include <SFML/System.hpp>
struct OB
{
    sf::Vector2f Position;
    sf::Vector2f Velocity={0,0};
    sf::Vector2i chunk;
    int radius,rotation=0,VelocityRotation=0;
    int Parent=-1;
    float temp=0,pressure=0;
    int Strength;
    int distanceConnection;
    float time=0;
    int ID;
    int Material;
};
