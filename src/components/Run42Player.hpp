#pragma once

#include "ecs/Component.hpp"

struct Run42PlayerComponent : ecs::IComponentBase
{
	enum class Position {
		Left, Middle, Right
	};
	enum class Movement {
		Running,
		Jumping,
	};

	Position CurrentPosition = Position::Middle;

	Movement CurrentMovement = Movement::Running;

	float Speed = 200.0f; /* Centimeter per second */

	float CurrentJumpDistance = 0.0f; /* Centimeter */
	float MaxJumpDistance = 150.0f;   /* Centimeter */
};
