#include <LiteMath.h>
#include <Image2d.h>

// Parallel processing
#include <omp.h>

#include "constants.h"
#include "scene.h"
#include "render.h"

using namespace LiteMath;
using namespace LiteImage;

namespace render {

    /// CPU ///
    static void pixel(Image2D<float4> &image, int2 coord);

}

///////////////////////////////////////////
///                 CPU                 ///
///////////////////////////////////////////

// Calculate pixel at the given image coord

#include <iostream>
void render::pixel(Image2D<float4> &image, int2 coord) {
    static const float AR = float(constants::width) / constants::height;

    float w = scene::camera->focal;
    float h = w / AR;

    float u = (coord.x + 0.5) / constants::width;
    float v = (coord.y + 0.5) / constants::height;

    float x = lerp( -w/2, w/2, u);
    float y = lerp( (float)h/2, (float)-h/2, v);
    float z = -1.0f;
    float3 ray = normalize( float3(x, y, z) );
    ray = scene::camera->view(ray, false);

    float3 position = float3(0.0f);
    position = scene::camera->view(position);

    float3 color = scene::raymarch(position, ray);
    image[coord] = to_float4(color, 1.0f);
}

void render::CPU(Image2D<float4> &image) {
    for (int pi = 0; pi < constants::height; pi++) {
        for (int pj = 0; pj < constants::width; pj++) {
            int2 coord(pj, pi);
            pixel(image, coord);
        }
    }
}

void render::OMP(Image2D<float4> &image) {
    #pragma omp parallel for
    for (int pi = 0; pi < constants::height; pi++) {
        for (int pj = 0; pj < constants::width; pj++) {
            int2 coord(pj, pi);
            pixel(image, coord);
        }
    }
}