#pragma once

#include <opfor/ecs/Component.hpp>

namespace opfor
{

struct DirectionalLightComponent : ecs::IComponentBase
{
    Vec3 Direction;
    Vec3 Color;
    float Intensity;
};

} // namespace opfor
