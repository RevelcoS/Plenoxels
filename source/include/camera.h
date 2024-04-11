#pragma once

#include <LiteMath.h>
#include "constants.h"

using namespace LiteMath;

struct Camera {
    float3 position;
    float3 direction;
    float3 up;
    float FOV;

    float4x4 transform;
    float focal;
    Camera(float3 position = float3(0.0f),
        float3 direction = float3(0.0f, 0.0f, -1.0f),
        float3 up = float3(0.0f, 1.0f, 0.0f),
        float FOV = 90);
    void update(void);
    float3 view(float3 vector, bool offset = true);
};