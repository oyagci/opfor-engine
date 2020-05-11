#pragma once

#include "lazy.hpp"
#include "ecs/Component.hpp"

struct TransformComponent : ecs::IComponentBase
{
	glm::vec3 position{0.0f, 0.0f, 0.0f};
	glm::vec3 direction;
	glm::vec3 scale{1.0f, 1.0f, 1.0f};

	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
};
