#include "../World.hpp"
#include "algorithm"
#include <cmath>
void World::Render(sf::RenderWindow& window,sf::Vector2f Position,float zoom){
    if (false)
    {
        int targetCX = Position.x / TILE_SIZE;
        int targetCY = Position.y / TILE_SIZE;
        int maxY = static_cast<int>(tileGravity.size());
        int maxX = static_cast<int>(tileGravity[0].size());
        float Zzoom = zoom;

        targetCX = std::clamp(targetCX, 0, maxX - 1);
        targetCY = std::clamp(targetCY, 0, maxY - 1);
        int view = static_cast<int>(8 * Zzoom);

        int startY = std::max(0, targetCY - view);
        int endY   = std::min(maxY, targetCY + view + 1);


        for (int y1 = startY; y1 < endY; y1++) {

            int startX = std::max(0, targetCX - view);
            int endX   = std::min(maxX, targetCX + view + 1);
            for (int x1 = startX; x1 < endX; x1++) {

                sf::RectangleShape d({TILE_SIZE,TILE_SIZE});
                float v = tileGravity[y1][x1] * 8.f;

                v = std::clamp(v, 0.f, 255.f);

                d.setFillColor(sf::Color(
                    static_cast<std::uint8_t>(v),
                    static_cast<std::uint8_t>(v),
                    static_cast<std::uint8_t>(v),
                    128
                ));
                d.setPosition({x1*TILE_SIZE,y1*TILE_SIZE});
                window.draw(d);
            }
        }
    }
    
    {
        sf::CircleShape Cir(0);
        int targetCX = Position.x / 64;
        int targetCY = Position.y / 64;
        int maxY = static_cast<int>(Chunks.size());
        int maxX = static_cast<int>(Chunks[0].size());
        float Zzoom = zoom;

        targetCX = std::clamp(targetCX, 0, maxX - 1);
        targetCY = std::clamp(targetCY, 0, maxY - 1);
        int view = static_cast<int>(8 * Zzoom);

        int startY = std::max(0, targetCY - view);
        int endY   = std::min(maxY, targetCY + view + 1);


        for (int y1 = startY; y1 < endY; y1++) {

            int startX = std::max(0, targetCX - view);
            int endX   = std::min(maxX, targetCX + view + 1);
            for (int x1 = startX; x1 < endX; x1++) {

                auto& chunk = Chunks[y1][x1];

                for (auto& objectA : chunk.Objects)
                {
                    // =========================
                    // KULA
                    // =========================
                    Cir.setRadius(static_cast<float>(objectA->radius));
                    Cir.setPosition(objectA->Position);

                    Cir.setFillColor(sf::Color(
                        static_cast<std::uint8_t>(objectA->Material * 3),
                        static_cast<std::uint8_t>(objectA->Material % 255),
                        static_cast<std::uint8_t>(std::abs(std::sin(objectA->Material)) * 255.f)
                    ));

                    window.draw(Cir);

                    // =========================
                    // LINIA PRĘDKOŚCI
                    // =========================
                    sf::VertexArray line(sf::PrimitiveType::Lines, 2);

                    line[0].position = objectA->Position;

                    // długość wizualizacji prędkości
                    sf::Vector2f vel = objectA->Velocity * 0.2f;

                    line[1].position = objectA->Position + vel;

                    // kolor zależny od prędkości
                    float speed =
                        std::sqrt(objectA->Velocity.x * objectA->Velocity.x +
                                objectA->Velocity.y * objectA->Velocity.y);

                    std::uint8_t c = static_cast<std::uint8_t>(std::min(speed * 10.f, 255.f));

                    line[0].color = sf::Color(c, 255 - c, 80);
                    line[1].color = sf::Color(c, 255 - c, 80);

                    //window.draw(line);
                }
                sf::RectangleShape d({64,64});
                d.setFillColor({0,0,0,0});
                d.setOutlineColor({255,0,0,32});
                d.setPosition({x1*64,y1*64});
                d.setOutlineThickness(1);
                window.draw(d);
            }
        }
    }
}