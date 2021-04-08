#pragma once

#include <opfor/ecs/Component.hpp>
#include <opfor/ecs/Entity.hpp>
#include <opfor/core/base.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Quat.hpp>
#include <opfor/editor/EditorVisibleProperty.hpp>
#include <opfor/editor/CustomEditorProperty.hpp>
#include <generated/TransformComponent.rfk.h>

namespace opfor OP4NAMESPACE()
{

/**
 * @brief Describe how an object should be positioned
 */
struct OP4STRUCT() TransformComponent : ecs::IComponentBase
{
    OP4FIELD()
    opfor::String name;

    OP4FIELD(EditorVisible)
    opfor::Vec3 position{0.0f, 0.0f, 0.0f};

    OP4FIELD(EditorVisible)
    opfor::Quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

    OP4FIELD(EditorVisible)
    opfor::Vec3 scale{1.0f, 1.0f, 1.0f};

    OP4FIELD()
    /**
     * @brief The entity this component is attached to (owner)
     */
    ecs::IEntityBase &entity;

    OP4FIELD()
    opfor::Optional<opfor::Ref<TransformComponent>> parent;

    TransformComponent(ecs::IEntityBase &owner) : entity(owner)
    {
    }

    TransformComponent_GENERATED
};

} // namespace opfor

File_GENERATED

