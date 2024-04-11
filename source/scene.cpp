#include <LiteMath.h>

// scene::load
#include <string>
#include <fstream>
#include <sstream>

#include "constants.h"
#include "scene.h"
#include "grid.h"

using namespace LiteMath;

namespace scene {
    Grid *grid;
    Camera *camera;
}

#include <iostream>
float3 scene::raymarch(float3 position, float3 ray) {
    float2 bounds = grid->intersection(position, ray);
    float tNear = bounds.x, tFar = bounds.y;

    float3 total = float3(0.0f);
    if (tNear > tFar || tNear < 0.0f || tFar > 100.0f)
        return total;

    float t = tNear;
    float dt = (tFar - tNear) / constants::bins;

    float sum = 0.0f; // Accumulated transmittance sum

    while (t < tFar) {
        float4 value = scene::grid->evaluate(position + t * ray, ray);
        float3 color = max(float3(value.x, value.y, value.z), float3(0.0f));
        float density = value.w;

        float alpha = 1.0f - exp(-density * dt);
        sum += density * dt;
        float T = exp(-sum); // Accumulated transmittance
        total += T * alpha * color;

        t += dt;
    }

    return clamp(total, float3(0.0f), float3(1.0f));
}

void scene::load(const char *model, const char *scene) {
    scene::grid = new Grid();
    scene::grid->load(model);

    scene::camera = new Camera();

    std::ifstream file(scene);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream input(line);
        std::string cmd;
        input >> cmd;

        float3 vector;
        float scalar;
        if (cmd == "Position") {
            input >> vector.x >> vector.y >> vector.z;
            scene::camera->position = vector;
        }
        else if (cmd == "Direction") {
            input >> vector.x >> vector.y >> vector.z;
            scene::camera->direction = vector;
        }
        else if (cmd == "Up") {
            input >> vector.x >> vector.y >> vector.z;
            scene::camera->up = vector;
        }
        else if (cmd == "FOV") {
            input >> scalar;
            scene::camera->FOV = scalar;
        }
    }

    // Update camera transform
    scene::camera->update();
}