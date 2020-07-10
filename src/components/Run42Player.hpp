#pragma once

#include "ecs/Component.hpp"

struct Run42PlayerComponent : ecs::IComponentBase
{
	enum class Position {
		Left, Middle, Right
	};
	Position CurrentPosition = Position::Middle;
};
