#pragma once 
#include "Ob/OB.hpp"
#include "Chunk/Chunk.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
struct World{
public:
    float TILE_SIZE=16;
    std::vector<std::vector<float>> tileGravity;
    std::vector<std::vector<Chunk>> Chunks;
    float timeScale = 1.f;
    void Step(float &dt,sf::RenderWindow&window,sf::View &camera,float zoom);
    void OBChunkRessign();
    void CollisionAStep(float dt);
    void init();
    void Render(sf::RenderWindow& window,sf::Vector2f Position,float zoom);
};