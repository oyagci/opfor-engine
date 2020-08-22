//
// Created by mploux on 20/12/18.
//

#pragma once

#include <vector>
#include <algorithm>
#include "Mouse.hpp"
#include "graphics/Display.hpp"

namespace lazy
{
	namespace inputs
	{
		class Keyboard;
		class Mouse;

		struct input
		{
		private:
			static Keyboard *keyboard;
			static Mouse *mouse;

			static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
			static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		public:
			static void init(const graphics::Display &display);
			static void clean();
			static void update();

			static Keyboard &getKeyboard() { return *keyboard; }
			static Mouse &getMouse() { return *mouse; }
		};
	}
}