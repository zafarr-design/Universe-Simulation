#pragma once
#include "../Ob/OB.hpp"
#include <memory>
#include <vector>

struct Chunk
{
    std::vector<std::unique_ptr<OB>> Objects;
};