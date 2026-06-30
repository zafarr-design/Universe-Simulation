#include "World.hpp"

void World::Step(float &dt,sf::RenderWindow&window,sf::View &camera,float zoom){
    OBChunkRessign();
    CollisionAStep(dt);
    Render(window,camera.getCenter(),zoom);
}