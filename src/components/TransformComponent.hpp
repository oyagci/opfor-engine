#pragma once

#include "lazy.hpp"
#include "ecs/Component.hpp"

struct TransformComponent : ecs::IComponentBase
{
	glm::vec3 position;
	glm::vec3 direction;

	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
};
