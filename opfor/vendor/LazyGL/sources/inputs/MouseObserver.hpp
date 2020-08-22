#pragma once

#include <glm/vec4.hpp>

namespace lazy
{
	namespace inputs
	{
		class IMouseObserver
		{
		public:
			virtual ~IMouseObserver() {}
			virtual void onClickUpInside() = 0;
			virtual void onHover(bool) = 0;
			virtual glm::vec4 getObservedArea() const = 0;
			virtual bool canBeClicked() const = 0;
		};
	}
}
