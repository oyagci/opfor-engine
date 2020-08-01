#pragma once

#include "engine/core/base.hpp"
#include <type_traits>

namespace opfor {

enum class EventType
{
	None = 0,
	WindowResized, WindowClosed, WindowFocused, WindowLostFocus, WindowMoved,
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased,
	MouseMoved, MouseScrolled
};

enum EventCategory
{
	EventCategoryApplication = BIT(0),
	EventCategoryInput       = BIT(1),
	EventCategoryKeyboard    = BIT(2),
	EventCategoryMouse       = BIT(3),
	EventCategoryMouseButton = BIT(4),
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
								virtual EventType GetEventType() const override { return GetStaticType(); } \
								virtual const char *GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class Event
{
	public:
		virtual ~Event() {}

		virtual EventType GetEventType() const = 0;
		virtual const char *GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool Handled = false;
};

class EventDispatcher
{
private:
	Event &_Event;

public:
	template <typename T>
	using EventFn = std::function<bool(T&)>;

	EventDispatcher(Event &e)
		: _Event(e) {}

	template <typename T>
	bool DispatchIf(EventFn<T> func)
	{
		static_assert(std::is_base_of<Event, T>::value);

		if (_Event.GetEventType() == T::GetStaticType()) {
			_Event.Handled = func(*dynamic_cast<T*>(&_Event));
			return true;
		}

		return false;
	}
};

}
