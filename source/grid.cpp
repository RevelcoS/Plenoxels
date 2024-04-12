#include <LiteMath.h>

// Loading grid
#include <cstdio>
#include <unistd.h>
#include <iostream>

#include "constants.h"
#include "grid.h"

using namespace LiteMath;

// From Mitsuba 3
static void shcoeffs(const float3 &d, float *out) {
    float x = d.x, y = d.y, z = d.z, z2 = z * z;
    float c0, c1, s0, s1, tmp_a, tmp_b, tmp_c;

    out[0] = 0.28209479177387814;
    out[2] = z * 0.488602511902919923;
    out[6] = z2 * 0.94617469575756008 + -0.315391565252520045;
    c0 = x;
    s0 = y;

    tmp_a = -0.488602511902919978;
    out[3] = tmp_a * c0;
    out[1] = tmp_a * s0;
    tmp_b = z * -1.09254843059207896;
    out[7] = tmp_b * c0;
    out[5] = tmp_b * s0;
    c1 = x * c0 - y * s0;
    s1 = x * s0 + y * c0;

    tmp_c = 0.546274215296039478;
    out[8] = tmp_c * c1;
    out[4] = tmp_c * s1;
}

static float evalsh(float* sh, float3 normal) {
    float coeffs[constants::shsize];
    shcoeffs(normal, coeffs);

    float sum = 0.0f;
    for (int i = 0; i < constants::shsize; i++)
        sum += sh[i] * coeffs[i];

    return sum;
}

// https://github.com/msu-graphics-group/LiteRF/blob/main/example_tracer/example_tracer.cpp
float2 Grid::intersection(float3 position, float3 ray) {
    ray.x = 1.0f / ray.x;
    ray.y = 1.0f / ray.y;
    ray.z = 1.0f / ray.z;

    float lo = ray.x * (this->minPoint.x - position.x);
    float hi = ray.x * (this->maxPoint.x - position.x);

    float tmin = std::min(lo, hi);
    float tmax = std::max(lo, hi);

    float lo1 = ray.y * (this->minPoint.y - position.y);
    float hi1 = ray.y * (this->maxPoint.y - position.y);

    tmin = std::max(tmin, std::min(lo1, hi1));
    tmax = std::min(tmax, std::max(lo1, hi1));

    float lo2 = ray.z * (this->minPoint.z - position.z);
    float hi2 = ray.z * (this->maxPoint.z - position.z);

    tmin = std::max(tmin, std::min(lo2, hi2));
    tmax = std::min(tmax, std::max(lo2, hi2));

    return float2(tmin, tmax);
}

// Returns color and density
float4 Grid::evaluate(float3 position, float3 ray) {
    int3 coord = this->coord(position);
    int3 xoff = int3(1, 0, 0);
    int3 yoff = int3(0, 1, 0);
    int3 zoff = int3(0, 0, 1);

    float3 p0 = this->position(coord);
    float3 p1 = this->position(coord + xoff + yoff + zoff);
    float3 sample = (position - p0) / (p1 - p0);

    float4 c000 = this->get(coord, ray);
    float4 c001 = this->get(coord + zoff, ray);
    float4 c010 = this->get(coord + yoff, ray);
    float4 c011 = this->get(coord + yoff + zoff, ray);
    float4 c100 = this->get(coord + xoff, ray);
    float4 c101 = this->get(coord + xoff + zoff, ray);
    float4 c110 = this->get(coord + xoff + yoff, ray);
    float4 c111 = this->get(coord + xoff + yoff + zoff, ray);

    float4 c00 = c000 * (1 - sample.x) + c100 * sample.x;
    float4 c01 = c001 * (1 - sample.x) + c101 * sample.x;
    float4 c10 = c010 * (1 - sample.x) + c110 * sample.x;
    float4 c11 = c011 * (1 - sample.x) + c111 * sample.x;

    float4 c0 = c00 * (1 - sample.y) + c10 * sample.y;
    float4 c1 = c01 * (1 - sample.y) + c11 * sample.y;

    float4 c = c0 * (1 - sample.z) + c1 * sample.z;
    return c;
}

float4 Grid::get(int3 coord, float3 ray) {
    int index = constants::grid * constants::grid * coord.z + constants::grid * coord.y + coord.x;
    if (index > constants::total)
        return float4(0.0f);

    Cell cell = this->cells[index];

    float r = evalsh(cell.shr, ray);
    float g = evalsh(cell.shg, ray);
    float b = evalsh(cell.shb, ray);
    float3 color = float3( r, g, b );

    return to_float4(color, cell.density);
}

float3 Grid::position(int3 coord) {
    float3 sample = float3(coord) / constants::grid;
    return minPoint + (maxPoint - minPoint) * sample;
}

int3 Grid::coord(float3 position) {
    return int3( floor( (constants::grid - 1) * (position + 1.0f) / 2 ) );
}

void Grid::load(const char *path) {
    FILE *file = std::fopen(path, "rb");
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
    }

    if (std::fread(this->cells, sizeof( *(this->cells) ), constants::total, file) != constants::total) {
        std::cout << "Failed to read the file" << std::endl;
    }
}