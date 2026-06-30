#include "World.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

void World::init()
{
    int SizeY = 60;
    int SizeX = 60;
    sf::Vector2f Start={SizeX*32,SizeY*32};
    tileGravity = std::vector<std::vector<float>>(SizeY*64/TILE_SIZE, std::vector<float>(SizeX*64/TILE_SIZE, 0.0f));
    bool R=false;
    srand(static_cast<unsigned>(time(nullptr)));
    Chunks.clear();

    for (int cy = 0; cy < SizeY; cy++)
    {
        std::vector<Chunk> C;

        for (int cx = 0; cx < SizeX; cx++)
        {
            Chunk chunk;

            for (int i = 0; i < 10000 && !R; i++)
            {
                auto ob = std::make_unique<OB>();

                ob->Position ={rand()%SizeX*64,rand()%SizeY*64};
                ob->radius = 5;
                ob->ID=i;
                ob->Velocity = {rand()%30-10,rand()%30-10};
                ob->Material=rand()%1000;

                chunk.Objects.push_back(std::move(ob));
            }
            R=true;

            C.push_back(std::move(chunk));
        }

        Chunks.push_back(std::move(C));
    }
}