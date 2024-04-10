#include <LiteMath.h>
#include <Image2d.h>

using namespace LiteMath;
using namespace LiteImage;

namespace render {
    void CPU(Image2D<float4> &image);
    void OMP(Image2D<float4> &image);
}