#pragma once

#include "ecs/Component.hpp"
#include <vector>
#include <string>

struct ModelComponent : ecs::IComponentBase
{
	std::string Name;
	unsigned int Shader;
	std::vector<unsigned int> Meshes;
};
