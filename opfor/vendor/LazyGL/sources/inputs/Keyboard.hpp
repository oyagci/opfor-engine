//
// Created by mploux on 20/12/18.
//

#pragma once

#include "graphics/Display.hpp"
#include <vector>
#include <algorithm>

namespace lazy
{
	namespace inputs
	{
		class Keyboard
		{
		private:
			const graphics::Display	&display;

			std::vector<int> 	keys;
			std::vector<int> 	downKeys;
			std::vector<int> 	upKeys;

		public:
			Keyboard(const graphics::Display &display);
			~Keyboard();

			void update();

			bool getKey(int key) const { return (std::find(keys.begin(), keys.end(), key) != keys.end()); };
			bool getKeyDown(int key) const { return (std::find(downKeys.begin(), downKeys.end(), key) != downKeys.end()); };
			bool getKeyUp(int key) const { return (std::find(upKeys.begin(), upKeys.end(), key) != upKeys.end()); };

			void keyCallback(int key, int scancode, int action, int mods);
		};
	}
}
