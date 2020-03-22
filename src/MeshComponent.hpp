#pragma once

#include <memory>
#include "ecs/ecs.hpp"
#include "lazy.hpp"

struct MeshComponent : ecs::IComponent
{
	lazy::graphics::Mesh mesh;
};
