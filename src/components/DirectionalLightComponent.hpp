#pragma once

#include <glm/vec3.hpp>
#include "ecs/Component.hpp"

struct DirectionalLightComponent : ecs::IComponentBase
{
	glm::vec3 Direction;
	glm::vec3 Color;
	float Intensity;
};
