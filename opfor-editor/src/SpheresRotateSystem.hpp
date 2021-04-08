#pragma once

#include "components/TransformComponent.hpp"
#include "opfor/ecs/System.hpp"
#include <glm/ext/quaternion_transform.hpp>

struct SphereComponent : opfor::ecs::IComponentBase
{ /* Tag Component */
};

class SpheresRotateSystem : public opfor::ecs::ComponentSystem
{
  public:
    void OnUpdate(float dt) override
    {
        auto entt = GetEntities<SphereComponent, opfor::TransformComponent>();

        for (auto &e : entt)
        {
            auto [sphere, transform] = e->GetAll();

            glm::quat newRotation = transform.rotation;
            newRotation = glm::rotate(newRotation, 1.0f * dt, glm::vec3(0, 1, 0));
            e->Get<opfor::TransformComponent>().rotation = newRotation;
        }
    }
};
