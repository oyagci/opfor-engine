#pragma once

#include "ecs/Component.hpp"
#include <glm/vec2.hpp>

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
