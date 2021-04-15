#pragma once

#include "components/TransformComponent.hpp"
#include "opfor/ecs/System.hpp"
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

            opfor::Quat newRotation = transform.rotation;
            newRotation.Rotate(opfor::Vec3(0.f, 1.f, 0.f), 1.0f * dt);
            e->Get<opfor::TransformComponent>().rotation = newRotation;
        }
    }
};
