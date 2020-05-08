#pragma once

#include "ecs/Component.hpp"
#include <glm/mat4x4.hpp>

struct PlayerCameraComponent : ecs::IComponentBase
{
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewProjection;
};
