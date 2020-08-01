#pragma once

#include <string>
#include <optional>
#include "ecs/Component.hpp"
#include "LuaRuntime.hpp"

struct LuaScriptComponent : ecs::IComponentBase
{
	std::string Path;
	LuaRuntime Runtime;
};
