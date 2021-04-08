#pragma once

#include <opfor/ecs/Component.hpp>
#include <glm/vec3.hpp>

namespace opfor
{

struct DirectionalLightComponent : ecs::IComponentBase
{
    glm::vec3 Direction;
    glm::vec3 Color;
    float Intensity;
};

} // namespace opfor
