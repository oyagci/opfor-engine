#pragma once

#include "SceneComponent.hpp"
#include "TextureManager.hpp"
#include "lazy.hpp"

using lazy::graphics::Mesh;

class IUIScene;

class MainMenuBackground : public ASceneComponent
{
public:
	MainMenuBackground(IUIScene *parent);

	void draw(Shader &shader) override
	{
		TextureManager::instance().bind("MenuBackground", GL_TEXTURE0);
		_background.draw();
	}

private:
	glm::vec2 _size;
	Mesh _background;
};
