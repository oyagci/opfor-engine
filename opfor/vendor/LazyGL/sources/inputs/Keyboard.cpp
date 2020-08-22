//
// Created by mploux on 20/12/18.
//

#include "Keyboard.hpp"

namespace lazy
{
	namespace inputs
	{
		Keyboard::Keyboard(const graphics::Display &display)
			: display(display)
		{

		}

		Keyboard::~Keyboard()
		{

		}

		void Keyboard::update()
		{
			for (int i = 0; i < downKeys.size(); i++)
			{
				int key = downKeys[i];
				if (!getKey(key))
					keys.push_back(key);
				else
					downKeys.erase(downKeys.begin() + i);
			}
			for (int i = 0; i < upKeys.size(); i++)
			{
				int key = upKeys[i];
				if (!getKey(key))
					upKeys.erase(upKeys.begin() + i);
			}
			for (int i = 0; i < keys.size(); i++)
			{
				int key = keys[i];
				if (getKeyUp(key))
					keys.erase(keys.begin() + i);
			}
		}

		void Keyboard::keyCallback(int key, int scancode, int action, int mods)
		{
			if (action == GLFW_PRESS)
			{
				if (!getKeyDown(key))
					downKeys.push_back(key);
			}
			else if (action == GLFW_RELEASE)
			{
				if (!getKeyUp(key))
					upKeys.push_back(key);
				
				auto downKeyIndex = std::find(downKeys.begin(), downKeys.end(), key);
				if (downKeyIndex != downKeys.end())
					downKeys.erase(downKeyIndex);
			}
		}
	}
}