#pragma once

#include "ecs/System.hpp"
#include "components/TransformComponent.hpp"
#include "components/BoxColliderComponent.hpp"

class Collision2DSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		auto entities = GetEntities<TransformComponent, BoxCollider2DComponent>();

		if (entities.size() == 0) { return ; }

		for (auto const &entity : entities) {

			auto &collider = entity->Get<BoxCollider2DComponent>();
			auto position = entity->Get<TransformComponent>().position;

			glm::vec2 colliderStart = collider.start + glm::vec2(position.x, position.z);
			glm::vec2 colliderSize  = collider.end;

			// Reset collision status
			collider.IsColliding = false;

			// Check for collision
			for (auto const &otherEntity : entities) {

				// We don't want to compare it to itself.
				// It would not make any sense
				if (&otherEntity == &entity) {
					continue ;
				}

				auto const otherCollider = otherEntity->Get<BoxCollider2DComponent>();
				auto const otherPosition = otherEntity->Get<TransformComponent>().position;

				glm::vec2 otherColliderStart = otherCollider.start + glm::vec2(otherPosition.x, otherPosition.z);
				glm::vec2 otherColliderSize  = otherCollider.end;

				if (                 colliderStart.x < otherColliderStart.x + otherColliderSize.x &&
					colliderStart.x + colliderSize.x > otherColliderStart.x &&
					                 colliderStart.y < otherColliderStart.y + otherColliderSize.y &&
					colliderStart.y + colliderSize.y > otherColliderStart.y) {

					// Collision Detected!
					collider.IsColliding = true;

//					fmt::print("======================================================\n");
//					fmt::print("(x: {}, y: {}, w: {}, h: {}) (x: {}, y: {}, w: {}, h: {})\n",
//							colliderStart.x, colliderStart.y, colliderSize.x, colliderSize.y,
//							otherColliderStart.x, otherColliderStart.y, otherColliderSize.x, otherColliderSize.y);
//					fmt::print("({}, {})\n", otherPosition.x, otherPosition.y);
				}
			}
		}
	}
};
