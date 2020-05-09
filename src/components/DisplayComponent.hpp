#pragma once

#include "ecs/Component.hpp"
#include "lazy.hpp"

struct DisplayComponent : ecs::IComponentBase
{
	lazy::graphics::Display *display;
};
