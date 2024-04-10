#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <LiteMath.h>

#include "constants.h"
#include "scene.h"

using namespace LiteMath;

namespace scene {
    Cell cells[constants::grid * constants::grid * constants::grid];
}

float3 scene::raymarch(float3 position, float3 ray) {
    return float3(1.0f);
}

void scene::load(const char *path) {
    struct stat stats;
    if (stat(path, &stats) == -1) {
        std::cout << "Failed to read file size" << std::endl;
    }

    uint total = stats.st_size / sizeof(*scene::cells);

    FILE *file = fopen(path, "rb");
    if (!file) {
        std::cout << "Failed to open file" << std::endl;
    }

    fread(scene::cells, sizeof(*scene::cells), total, file);
}