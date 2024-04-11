#pragma once

#include <LiteMath.h>

using namespace LiteMath;

namespace constants {
    static const char *title    = "Plenoxels";          // Project title
    const uint width            = 1024;                 // Screen width
    const uint height           = 768;                  // Screen height
    const uint bins             = 100;                  // Integration partitions
    const float gamma           = 1.0f;                 // Gamma correction
    constexpr uint shsize       = 9;                    // Spherical harmonics
    constexpr uint grid         = 128;                  // Grid size
    constexpr uint total        = grid * grid * grid;   // Total cells
}