#pragma once

#include <LiteMath.h>

#include "grid.h"
#include "camera.h"

using namespace LiteMath;

namespace scene {
    extern Grid *grid;
    extern Camera *camera;
    float3 raymarch(float3 position, float3 ray);
    void load(const char *model, const char *scene);
};