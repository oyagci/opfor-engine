//
// Created by mploux on 20/12/18.
//

#include "Input.hpp"
#include "Keyboard.hpp"

namespace lazy
{
	namespace inputs
	{
		Keyboard *input::keyboard;
		Mouse *input::mouse;

		void input::init(const graphics::Display &display)
		{
			keyboard = new Keyboard(display);
			mouse = new Mouse(display);

			glfwSetKeyCallback(display.getWindow(), keyCallback);
			glfwSetCursorPosCallback(display.getWindow(), cursorPositionCallback);
			glfwSetMouseButtonCallback(display.getWindow(), mouseButtonCallback);
		}

		void input::update()
		{
			keyboard->update();
			mouse->update();
		}

		void input::clean()
		{
			delete keyboard;
			delete mouse;
		}

		void input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			keyboard->keyCallback(key, scancode, action, mods);
		}

		void input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
		{
			mouse->positionCallback(xpos, ypos);
		}

		void input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			mouse->buttonCallback(button, action, mods);
		}

	}
}