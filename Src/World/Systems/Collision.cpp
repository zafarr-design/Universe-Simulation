#include "../World.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstring>

namespace
{
    inline float Clamp(float v, float lo, float hi)
    {
        return std::max(lo, std::min(hi, v));
    }
}

void World::CollisionAStep(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) timeScale = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M)) timeScale = 2.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) timeScale = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B)) timeScale = 0.1f;

    int chunkCountY = static_cast<int>(Chunks.size());
    int chunkCountX = chunkCountY > 0 ? static_cast<int>(Chunks[0].size()) : 0;

    dt *= timeScale;
    if (dt <= 0.f) return;

    const float gravityStrength = 1.f;
    const float invTileSize = 1.f / TILE_SIZE;


    int gravitySizeY = static_cast<int>(tileGravity.size());
    int gravitySizeX = gravitySizeY > 0 ? static_cast<int>(tileGravity[0].size()) : 0;

    if (gravitySizeY <= 0 || gravitySizeX <= 0) return;

    for (auto& row : tileGravity)
    {
        for (auto& cell : row)
        {
            cell *= 0.9;
        }
    }

    for (int y = 0; y < chunkCountY; ++y)
    {
        for (int x = 0; x < chunkCountX; ++x)
        {
            for (auto& object : Chunks[y][x].Objects)
            {
                int targetCX = (object->Position.x) / TILE_SIZE;
                int targetCY = (object->Position.y) / TILE_SIZE;
                int maxY = static_cast<int>(tileGravity.size());
                int maxX = static_cast<int>(tileGravity[0].size());
                targetCX = std::clamp(targetCX, 0, maxX - 1);
                targetCY = std::clamp(targetCY, 0, maxY - 1);
                tileGravity[targetCY][targetCX] += std::clamp(object->Material*object->radius,10,100);
            }
        }
    }

    static thread_local std::vector<std::vector<float>> blurBuffer;
    blurBuffer = tileGravity;

    for (int pass = 0; pass < 5; ++pass)
    {
        for (int y = 1; y < gravitySizeY - 1; ++y)
        {
            const float* prev = tileGravity[y - 1].data();
            const float* curr = tileGravity[y].data();
            const float* next = tileGravity[y + 1].data();
            float* dest = blurBuffer[y].data();

            for (int x = 1; x < gravitySizeX - 1; ++x)
            {
                float center = curr[x];

                float laplacian =
                    prev[x] + next[x] +
                    curr[x - 1] + curr[x + 1] -
                    center * 4.0f;

                float diffused = center + laplacian * 0.15f;

                dest[x] = diffused * 0.9985f;
            }
        }
        std::swap(tileGravity, blurBuffer);
    }



    for (int y = 0; y < chunkCountY; ++y)
    {
        for (int x = 0; x < chunkCountX; ++x)
        {
            for (auto& object : Chunks[y][x].Objects)
            {
                int objTileX = static_cast<int>(object->Position.x * invTileSize);
                int objTileY = static_cast<int>(object->Position.y * invTileSize);

                if (objTileY > 0 && objTileX > 0 &&
                    objTileY < gravitySizeY - 1 &&
                    objTileX < gravitySizeX - 1)
                {
                    float ax = (tileGravity[objTileY][objTileX + 1] -
                                tileGravity[objTileY][objTileX - 1]) * gravityStrength;
                    float ay = (tileGravity[objTileY + 1][objTileX] -
                                tileGravity[objTileY - 1][objTileX]) * gravityStrength;


                    object->Velocity.x += ax * dt;
                    object->Velocity.y += ay * dt;
                }

                object->Position += object->Velocity * dt;
            }
        }
    }

    const int iterations = 11; 

    static const int neighborOffsets[4][2] = {
        {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    auto resolvePair = [&](OB& a, OB& b)
    {
        if (&a == &b) return;

        sf::Vector2f delta = b.Position - a.Position;
        float dist2 = delta.x * delta.x + delta.y * delta.y;
        float minDist = a.radius + b.radius;

        if (dist2 >= minDist * minDist || dist2 <= 0.0001f) return;

        float dist = std::sqrt(dist2);
        sf::Vector2f normal = delta / dist;
        float penetration = minDist - dist;

  
        float massA = std::clamp(a.Material,1,100) * a.radius;
        float massB = std::clamp(b.Material,1,100) * b.radius;
        float invMassA = 1.f / massA;
        float invMassB = 1.f / massB;
        float invMassSum = invMassA + invMassB;

        const float correctionStrength = 0.85f;
        sf::Vector2f correction = normal * (penetration * correctionStrength / invMassSum);
        a.Position -= correction * invMassA;
        b.Position += correction * invMassB;

        sf::Vector2f relVel = b.Velocity - a.Velocity;
        float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

        if (velAlongNormal < 0.f)
        {
            const float restitution = 0.9f;

            float impulse = -(1.f + restitution) * velAlongNormal / invMassSum;
            sf::Vector2f impulseVec = impulse * normal;

            a.Velocity -= impulseVec * invMassA;
            b.Velocity += impulseVec * invMassB;
        }
    };

    for (int iter = 0; iter < iterations; ++iter)
    {
        for (int y = 0; y < chunkCountY; ++y)
        {
            for (int x = 0; x < chunkCountX; ++x)
            {
                auto& objsA = Chunks[y][x].Objects;
                const size_t countA = objsA.size();

                for (size_t i = 0; i < countA; ++i)
                    for (size_t j = i + 1; j < countA; ++j)
                        resolvePair(*objsA[i], *objsA[j]);

                for (auto& off : neighborOffsets)
                {
                    int ny = y + off[0];
                    int nx = x + off[1];
                    if (ny < 0 || ny >= chunkCountY || nx < 0 || nx >= chunkCountX) continue;

                    auto& objsB = Chunks[ny][nx].Objects;
                    for (auto& objectA : objsA)
                        for (auto& objectB : objsB)
                            resolvePair(*objectA, *objectB);
                }
            }
        }
    }
}