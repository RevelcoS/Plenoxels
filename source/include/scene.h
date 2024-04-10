#pragma once

#include <LiteMath.h>

using namespace LiteMath;

namespace scene {
    struct Cell {
        float density;
        float shr[constants::shsize];
        float shg[constants::shsize];
        float shb[constants::shsize];
    };

    extern Cell cells[constants::grid * constants::grid * constants::grid];
    float3 raymarch(float3 position, float3 ray);
    void load(const char *path);
};