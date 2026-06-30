#include "../World.hpp"
#include <algorithm>

void World::OBChunkRessign()
{
    int chunkCountY = static_cast<int>(Chunks.size());
    int chunkCountX = chunkCountY > 0 ? static_cast<int>(Chunks[0].size()) : 0;

    const int worldW = chunkCountX * 64;
    const int worldH = chunkCountY * 64;

    for (int y = 0; y < chunkCountY; y++)
    {
        for (int x = 0; x < chunkCountX; x++)
        {
            Chunk& chunk = Chunks[y][x];
            std::vector<std::unique_ptr<OB>> remaining;

            for (auto& object : chunk.Objects)
            {
                // =========================
                // 🔥 ODBICIE OD GRANIC ŚWIATA
                // =========================

                if (object->Position.x < 0)
                {
                    object->Position.x = 0;
                    object->Velocity.x *= -1;
                }
                else if (object->Position.x > worldW)
                {
                    object->Position.x = worldW;
                    object->Velocity.x *= -1;
                }

                if (object->Position.y < 0)
                {
                    object->Position.y = 0;
                    object->Velocity.y *= -1;
                }
                else if (object->Position.y > worldH)
                {
                    object->Position.y = worldH;
                    object->Velocity.y *= -1;
                }


                int targetCX = object->Position.x / 64;
                int targetCY = object->Position.y / 64;

                targetCX = std::clamp(targetCX, 0, chunkCountX - 1);
                targetCY = std::clamp(targetCY, 0, chunkCountY - 1);

                if (targetCX == x && targetCY == y)
                {
                    remaining.push_back(std::move(object));
                }
                else
                {
                    Chunks[targetCY][targetCX].Objects.push_back(std::move(object));
                }
            }

            chunk.Objects = std::move(remaining);
        }
    }
}