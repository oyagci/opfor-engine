#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "opfor/core/base.hpp"
#include "ecs/Entity.hpp"

struct TransformComponent : ecs::IComponentBase
{
	opfor::String name;

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	TransformComponent *parent = nullptr;
	ecs::IEntityBase &entity;

	TransformComponent(ecs::IEntityBase &owner) : entity(owner)
	{
	}
};
