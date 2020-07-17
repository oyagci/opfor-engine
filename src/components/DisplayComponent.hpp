#pragma once

#include "ecs/Component.hpp"
#include "lazy.hpp"
#include "engine/core/Window.hpp"

struct DisplayComponent : ecs::IComponentBase
{
	opfor::IWindow *window;
};
