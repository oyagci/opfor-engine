#pragma once

#include "opfor/core/events/Event.h"

namespace opfor {

class Layer
{
public:
	virtual ~Layer() {}

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float) {}
	virtual void OnEvent(Event &) {}

	inline const std::string &GetName() const { return _DebugName; }

protected:
	std::string _DebugName;
};

}
