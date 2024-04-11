#include <LiteMath.h>

// Loading grid
#include <cstdio>
#include <unistd.h>
#include <iostream>

#include "constants.h"
#include "grid.h"

using namespace LiteMath;

// https://github.com/sarafridov/plenoxels/blob/main/sh.py
/*
static float3 evalsh(float3 sh[constants::shsize], float3 normal) {
    static float C0 = 0.28209479177387814;
    static float C1 = 0.4886025119029199;
    static float C2[5] = {
        1.0925484305920792,
        -1.0925484305920792,
        0.31539156525252005,
        -1.0925484305920792,
        0.5462742152960396
    };

    float x = normal.x;
    float y = normal.y;
    float z = normal.z;

    float3 result = C0 * sh[0];
    result = (result -
            C1 * y * sh[1] +
            C1 * z * sh[2] -
            C1 * x * sh[3]);

    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, yz = y * z, xz = x * z;
    result = (result +
            C2[0] * xy * sh[4] +
            C2[1] * yz * sh[5] +
            C2[2] * (2.0 * zz - xx - yy) * sh[6] +
            C2[3] * xz * sh[7] +
            C2[4] * (xx - yy) * sh[8]);

    return max(result, float3(0.0f));
}*/

/*
// https://github.com/nicknikolov/glsl-sh/blob/master/index.glsl
static float evalsh(float sh[constants::shsize], float3 normal) {
    float x = normal.x;
    float y = normal.y;
    float z = normal.z;

    float result = (
        sh[0] +

        sh[1] * x +
        sh[2] * y +
        sh[3] * z +

        sh[4] * z * x +
        sh[5] * y * z +
        sh[6] * y * x +
        sh[7] * (3.0 * z * z - 1.0) +
        sh[8] * (x*x - y*y)
    );

    return max(result, 0.0f);
}
*/

// From Mitsuba 3
static void evalsh2(const float3 &d, float *out) {
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

static float evalsh(float* sh, float3 rayDir) {
    float sh_coeffs[constants::shsize];
    evalsh2(rayDir, sh_coeffs);

    float sum = 0.0f;
    for (int i = 0; i < constants::shsize; i++)
    sum += sh[i] * sh_coeffs[i];

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
    Cell cell = this->get(coord);

    float r = evalsh(cell.shr, ray);
    float g = evalsh(cell.shg, ray);
    float b = evalsh(cell.shb, ray);
    float3 color = float3( r, g, b );

    return to_float4(color, cell.density);
}

Cell Grid::get(int3 coord) {
    int index = constants::grid * constants::grid * coord.z + constants::grid * coord.y + coord.x;
    return this->cells[index];
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