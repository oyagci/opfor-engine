#pragma once

#include "ecs/Component.hpp"
#include <vector>
#include <string>

struct ModelComponent : ecs::IComponentBase
{
	std::string Path{};
	unsigned int Shader = 0;
	std::vector<unsigned int> Meshes{};
};
