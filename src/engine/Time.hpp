#include <GLFW/glfw3.h>

namespace engine
{

class Time {
public:
	static Time &instance()
	{
		static Time t;

		return t;
	}

private:
	Time() {};
	Time(Time const &) = delete;
	void operator=(Time const&) = delete;

public:
	void update()
	{
		_lastFrame = glfwGetTime();
	}

	float getDeltaTime()
	{
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - _lastFrame;
		return deltaTime;
	};

private:
	float _lastFrame = 0.0f;
};

}
