#pragma once

#include "ecs/Component.hpp"
#include <vector>
#include <string>

struct ModelComponent : ecs::IComponentBase
{
	std::string path = "";
	unsigned int shader = 0;
	std::vector<unsigned int> meshes{};
};
