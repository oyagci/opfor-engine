#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>

struct DirectionalLightComponent : ecs::IComponentBase
{
    glm::vec3 Direction;
    glm::vec3 Color;
    float Intensity;
};
