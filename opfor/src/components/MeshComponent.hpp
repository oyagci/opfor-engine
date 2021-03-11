#pragma once

#include "ecs/ecs.hpp"
#include "renderer.hpp"
#include <memory>

///
/// A component for meshes that holds data about the mesh
///
struct MeshComponent : ecs::IComponent
{
    unsigned int Id;
    unsigned int Shader;
};
