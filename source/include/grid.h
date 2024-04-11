#pragma once

#include "constants.h"

struct Cell {
    float density;
    float shr[constants::shsize];
    float shg[constants::shsize];
    float shb[constants::shsize];
};

struct Grid {
    Cell cells[constants::total];
    float3 minPoint = float3(-1, -1, -1);
    float3 maxPoint = float3( 1,  1,  1);

    float2 intersection(float3 position, float3 ray);
    float4 evaluate(float3 position, float3 ray);
    Cell get(int3 coord);
    static int3 coord(float3 position);
    void load(const char *path);
};