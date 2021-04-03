#pragma once

#include <opfor/ecs/Component.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/editor/EditorVisibleProperty.hpp>
#include <opfor/editor/EditorMinProperty.hpp>
#include <generated/PointLightComponent.rfk.h>

struct OP4STRUCT() PointLightComponent : ecs::IComponentBase
{
    OP4FIELD(EditorVisible)
    opfor::Vec3 Dir = {0.5f, 0.5f, 0.5f};

    OP4FIELD(EditorVisible)
    opfor::Vec3 Color = {1.0f, 1.0f, 0.8f};

    OP4FIELD(EditorVisible, Min(0.0f))
    float Intensity = 1000.0f;

    PointLightComponent_GENERATED
};

File_GENERATED
