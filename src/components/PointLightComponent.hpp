#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>

struct PointLightComponent : ecs::IComponentBase
{
	glm::vec3 Dir;
	glm::vec3 Color;

	float Intensity = 100.0f;
};
