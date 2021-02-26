#pragma once

#include "ecs/Component.hpp"
#include <vector>
#include <string>
#include "renderer/Material.hpp"

struct ModelComponent : ecs::IComponentBase
{
	std::string path = "";
	std::vector<unsigned int> meshes{};
	opfor::Material material;
};
