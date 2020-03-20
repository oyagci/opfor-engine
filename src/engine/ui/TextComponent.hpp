#pragma once

#include "SceneComponent.hpp"
#include "Anchor.hpp"
#include <glm/glm.hpp>
#include <string>

using anchor::Anchor;

class TextComponent : public ASceneComponent
{
public:
	TextComponent(IUIScene *scene) : ASceneComponent(scene)
	{
	}

	void update() override
	{
	}

	void draw(Shader &shader) override
	{
	}
};
