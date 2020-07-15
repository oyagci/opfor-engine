#pragma once

#include <string>
#include "ecs/Component.hpp"
#include "ui/Anchor.hpp"

struct TextComponent : ecs::IComponentBase
{
	std::string Text = "";
	float Scale = 1.0f;
	glm::vec3 Color;
	anchor::Anchor Anchor;

	static TextComponent New(std::string text, float scale = 1.0f, glm::vec3 color = { 1.0f, 1.0f, 1.0f }, anchor::Anchor anchor = anchor::Anchor::BottomLeft)
	{
		TextComponent textComponent;
			textComponent.Text = text;
			textComponent.Color = color;
			textComponent.Scale = scale;
			textComponent.Anchor = anchor;

		return textComponent;
	}
};
