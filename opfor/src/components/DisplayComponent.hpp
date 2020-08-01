#pragma once

#include "ecs/Component.hpp"
#include "lazy.hpp"
#include "opfor/core/Window.hpp"

struct DisplayComponent : ecs::IComponentBase
{
	opfor::IWindow *window;
};
