#include "Anchor.hpp"

namespace anchor
{

glm::vec2 calculateOffset(Anchor anchor, glm::vec2 size)
{
	glm::vec2 anchorOffset(0.0f, 0.0f);

	switch (anchor) {
	case Anchor::TopLeft:
		anchorOffset = -glm::vec2(0.0f, size.y);
		break ;
	case Anchor::Top:
		anchorOffset = -glm::vec2(size.x / 2.0f, size.y);
		break ;
	case Anchor::TopRight:
		anchorOffset = -glm::vec2(size.x, size.y);
		break ;
	case Anchor::Left:
		anchorOffset = -glm::vec2(0.0f, size.y / 2.0f);
		break ;
	case Anchor::Center:
		anchorOffset = -glm::vec2(size.x / 2.0f, size.y / 2.0f);
		break ;
	case Anchor::Right:
		anchorOffset = -glm::vec2(size.x, size.y / 2.0f);
		break ;
	case Anchor::BottomLeft:
		anchorOffset = -glm::vec2(0.0f, 0.0f);
		break ;
	case Anchor::Bottom:
		anchorOffset = -glm::vec2(size.x / 2.0f, 0.0f);
		break ;
	case Anchor::BottomRight:
		anchorOffset = -glm::vec2(size.x, 0.0f);
		break ;
	}

	return anchorOffset;
}

}
