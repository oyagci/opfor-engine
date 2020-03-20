#pragma once

#include <glm/glm.hpp>

namespace anchor {

enum class Anchor {
	TopLeft,
	Top,
	TopRight,
	Left,
	Center,
	Right,
	BottomLeft,
	Bottom,
	BottomRight,
};

glm::vec2 calculateOffset(Anchor anchor, glm::vec2 size);

}
