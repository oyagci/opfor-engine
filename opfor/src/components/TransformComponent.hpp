#pragma once

#include <opfor/ecs/Component.hpp>
#include <opfor/ecs/Entity.hpp>
#include <opfor/core/base.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

struct TransformComponent : ecs::IComponentBase
{
    opfor::String name;

    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    ecs::IEntityBase &entity;
    opfor::Optional<opfor::Ref<TransformComponent>> parent;

    TransformComponent(ecs::IEntityBase &owner) : entity(owner)
    {
    }
};
