#pragma once

#include "ecs/System.hpp"
#include "components/BoxColliderComponent.hpp"
#include "components/TransformComponent.hpp"

class Collision3DSystem : public ecs::ComponentSystem
{
public:
	void OnUpdate(float) override
	{
		auto entities = GetEntities<TransformComponent, BoxCollider3DComponent>();

		if (entities.size() == 0) { return ; }

		for (auto const &entity : entities) {

			auto &collider = entity->Get<BoxCollider3DComponent>();
			auto position = entity->Get<TransformComponent>().position;

			glm::vec3 const colliderStart = collider.position + position;
			glm::vec3 const colliderSize  = collider.size;

			// Reset collision status
			collider.IsColliding = false;

			// Check for collision
			for (auto const &otherEntity : entities) {

				// We don't want to compare it to itself.
				// It would not make any sense
				if (&otherEntity == &entity) {
					continue ;
				}

				auto const otherCollider = otherEntity->Get<BoxCollider3DComponent>();
				auto const otherPosition = otherEntity->Get<TransformComponent>().position;

				glm::vec3 const otherColliderStart = otherCollider.position + otherPosition;
				glm::vec3 const otherColliderSize  = otherCollider.size;

				if (                 colliderStart.x < otherColliderStart.x + otherColliderSize.x &&
					colliderStart.x + colliderSize.x > otherColliderStart.x &&
					                 colliderStart.y < otherColliderStart.y + otherColliderSize.y &&
					colliderStart.y + colliderSize.y > otherColliderStart.y &&
									 colliderStart.z < otherColliderStart.z + otherColliderSize.z &&
					colliderStart.z + colliderSize.z > otherColliderStart.z
					) {

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
