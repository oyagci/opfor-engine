#pragma once

#include <opfor/ecs/System.hpp>
#include <src/components/AnimationComponent.hpp>

namespace opfor
{
class AnimationSystem : public ecs::ComponentSystem
{
    float _last_update = 0.;

    void OnUpdate(float dt) final;

  public:
    void AddAnimation(AnimationComponent);
};

} // namespace opfor
