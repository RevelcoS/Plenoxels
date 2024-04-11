#include <LiteMath.h>

#include "constants.h"
#include "camera.h"

using namespace LiteMath;

Camera::Camera(float3 position, float3 direction, float3 up, float FOV) :
    position(position), direction(direction), up(up), FOV(FOV) {
    this->update();
}

void Camera::update() {
    // Transform
    float3 right = normalize(cross(this->direction, this->up));
    float3 up = normalize(cross(right, this->direction));
    float3 forward = normalize(this->direction);

    transform.set_col(0, to_float4(right, 0.0f));
    transform.set_col(1, to_float4(up, 0.0f));
    transform.set_col(2, to_float4(-forward, 0.0f));
    transform.set_col(3, to_float4(this->position, 1.0f));

    // Focal
    this->focal = 2.0f * tan(this->FOV * DEG_TO_RAD / 2);
}

float3 Camera::view(float3 vector, bool offset) {
    float4 extended = to_float4(vector, 0.0f);
    if (offset) extended.w = 1.0f;
    return to_float3(this->transform * extended);
}