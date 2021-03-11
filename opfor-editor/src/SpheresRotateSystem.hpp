#pragma once

#include "components/TransformComponent.hpp"
#include "opfor/ecs/System.hpp"
#include <glm/ext/quaternion_transform.hpp>

struct SphereComponent : ecs::IComponentBase
{ /* Tag Component */
};

class SpheresRotateSystem : public ecs::ComponentSystem
{
  public:
    void OnUpdate(float dt) override
    {
        auto entt = GetEntities<SphereComponent, TransformComponent>();

        for (auto &e : entt)
        {
            auto [sphere, transform] = e->GetAll();

            glm::quat newRotation = transform.rotation;
            newRotation = glm::rotate(newRotation, 1.0f * dt, glm::vec3(0, 1, 0));

            TransformComponent newTransform = transform;
            newTransform.rotation = newRotation;

            e->Set<TransformComponent>(newTransform);
        }
    }
};
