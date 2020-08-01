#pragma once

#include <lazy.hpp>

namespace opfor
{

class Time {
public:
	static Time &Get()
	{
		static Time t;

		return t;
	}

private:
	Time() {};
	Time(Time const &) = delete;
	void operator=(Time const&) = delete;

public:
	float getDeltaTime()
	{
		_lastFrame = _newFrame;
		_newFrame = glfwGetTime();
		return _newFrame - _lastFrame;
	};

private:
	float _lastFrame = 0.0f;
	float _newFrame = 0.0f;
};

}
