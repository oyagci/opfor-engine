#pragma once

#include <opfor/core/base.hpp>
#include <opfor/ecs/Component.hpp>
#include <opfor/editor/EditorReadOnlyProperty.hpp>
#include <opfor/editor/EditorVisibleProperty.hpp>
#include <generated/ModelComponent.rfk.h>

namespace opfor OP4NAMESPACE()
{

struct OP4STRUCT() ModelComponent : ecs::IComponentBase
{
    OP4FIELD(EditorVisible, ReadOnly)
    opfor::String path = "";

    OP4FIELD(EditorVisible, ReadOnly)
    unsigned int shader = 0;

    OP4FIELD()
    std::vector<unsigned int> meshes{};

    ModelComponent_GENERATED
};

} // namespace opfor

File_GENERATED
