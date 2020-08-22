//
// Created by mploux on 20/12/18.
//

#include "Mouse.hpp"

namespace lazy
{
	namespace inputs
	{
		Mouse::Mouse(const graphics::Display &display)
			: display(display)
		{

		}

		Mouse::~Mouse()
		{

		}

		void Mouse::update()
		{
			mouseVelocity = lastMousePosition - mousePosition;
			lastMousePosition = mousePosition;

			for (size_t i = 0; i < downButtons.size(); i++)
			{
				int key = downButtons[i].id;
				if (!getButton(key))
					buttons.push_back(downButtons[i]);
				else
					downButtons.erase(downButtons.begin() + i);
			}
			for (size_t i = 0; i < upButtons.size(); i++)
			{
				int key = upButtons[i].id;
				if (!getButton(key))
					upButtons.erase(upButtons.begin() + i);
				else
				{
					auto removeIndex = std::find(buttons.begin(), buttons.end(), key);
					if (removeIndex != buttons.end())
						buttons.erase(removeIndex);
				}
			}
			updateObservers();
		}

		void Mouse::updateObservers()
		{
			for (auto o : observers)
			{
				for (auto b : buttons)
				{
					if (b.id == 0)
					{
						auto area = o->getObservedArea();
						glm::vec2 viewportPos = glm::vec2(mousePosition.x, display.getHeight() - mousePosition.y);

						if (o->canBeClicked() &&
							viewportPos.x >= area.x && viewportPos.x <= area.x + area.z &&
							viewportPos.y >= area.y && viewportPos.y <= area.y + area.w)
						{
							o->onClickUpInside();
						}
					}
				}
				if (o->canBeClicked()) {
					auto area = o->getObservedArea();
					glm::vec2 viewportPos = glm::vec2(mousePosition.x, display.getHeight() - mousePosition.y);
					if (viewportPos.x >= area.x && viewportPos.x <= area.x + area.z &&
						viewportPos.y >= area.y && viewportPos.y <= area.y + area.w) {
						o->onHover(true);
					}
					else {
						o->onHover(false);
					}
				}
			}
		}

		void Mouse::positionCallback(double xpos, double ypos)
		{
			mousePosition.x = static_cast<float>(xpos);
			mousePosition.y = static_cast<float>(ypos);
		}

		void Mouse::buttonCallback(int button, int action, int mods)
		{
			(void)mods;
			if (action == GLFW_PRESS)
			{
				if (!getButtonDown(button))
				{
					Button b = { button, action, glm::vec2(mousePosition.x, display.getHeight() - mousePosition.y) };
					downButtons.push_back(b);
				}
			}
			else if (action == GLFW_RELEASE)
			{
				if (!getButtonUp(button))
				{
					Button b = { button, action, glm::vec2(mousePosition.x, display.getHeight() - mousePosition.y) };
					upButtons.push_back(b);
				}
			}
		}
	}
}
