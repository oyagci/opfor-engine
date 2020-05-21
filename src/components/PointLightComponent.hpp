#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>

struct PointLightComponent : ecs::IComponentBase
{
	glm::vec3 Dir;
	glm::vec3 Color;

	float constant = 1.0f;
	float linear = 0.0014f;
	float quadratic = 0.000007f;
};
