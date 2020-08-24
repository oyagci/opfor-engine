#pragma once

#include "Event.h"

namespace opfor {

class WindowResizeEvent : public Event
{
private:
	int _Width;
	int _Height;

public:
	WindowResizeEvent(int width, int height)
		: _Width(width), _Height(height) {}

	EVENT_CLASS_TYPE(WindowResized)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)

	inline auto GetWidth() const { return _Width; }
	inline auto GetHeight() const { return _Height; }
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() {}

	EVENT_CLASS_TYPE(WindowClosed)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class ViewportResizeEvent : public Event
{
private:
	int _Width;
	int _Height;

public:
	ViewportResizeEvent(int width, int height)
		: _Width(width), _Height(height) {}

	EVENT_CLASS_TYPE(ViewportResized)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)

	inline auto GetWidth() const { return _Width; }
	inline auto GetHeight() const { return _Height; }
};

}
