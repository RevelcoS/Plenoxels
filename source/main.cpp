#include "stb_image_write.h"

#include <LiteMath.h>
#include <Image2d.h>

// Test runtime
#include <chrono>
#include <iostream>

#include "constants.h"
#include "scene.h"
#include "render.h"

using namespace LiteMath;
using namespace LiteImage;

int main() {
    Image2D<float4> CPUimage(constants::width, constants::height);

    // Load scene
    std::cout << "...Loading scene" << std::endl;
    scene::load("assets/model.dat", "scene/camera.txt");

    /// CPU ///
    std::cout << "...Rendering" << std::endl;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;

    start = std::chrono::system_clock::now();
    render::CPU(CPUimage);
    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << "Render with CPU (1 thread):\t" << duration.count() << "s" << std::endl;

    // OpenMP
    start = std::chrono::system_clock::now();
    render::OMP(CPUimage);
    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << "Render with OpenMP (4 threads):\t" << duration.count() << "s" << std::endl;

    // Save CPU image
    SaveImage("out_cpu.png", CPUimage, constants::gamma);

    return 0;
}
