#pragma once

#include "ecs/Component.hpp"
#include "renderer.hpp"
#include "opfor/core/Window.hpp"

struct DisplayComponent : ecs::IComponentBase
{
	opfor::IWindow *window;
};
