#pragma once

#include "ecs/Component.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct BoxCollider2DComponent : ecs::IComponentBase
{
	glm::vec2 start;
	glm::vec2 end;

	bool IsColliding = false;

	static BoxCollider2DComponent New(glm::vec2, glm::vec2);
};

inline BoxCollider2DComponent BoxCollider2DComponent::New(glm::vec2 start, glm::vec2 end)
{
	BoxCollider2DComponent collider;
		collider.start = start;
		collider.end = end;
	return collider;
}

struct BoxCollider3DComponent : ecs::IComponentBase
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 size = { 0.0f, 0.0f, 0.0f };

	bool IsColliding = false;

	static BoxCollider3DComponent New(glm::vec3, glm::vec3);
};

inline BoxCollider3DComponent BoxCollider3DComponent::New(glm::vec3 position, glm::vec3 size)
{
	BoxCollider3DComponent box;
		box.position = position;
		box.size = size;
		box.IsColliding = false;
	return box;
}
