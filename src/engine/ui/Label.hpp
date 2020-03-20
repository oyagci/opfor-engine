#pragma once

#include "SceneComponent.hpp"
#include "TextRenderer.hpp"

class Label : public ASceneComponent
{
public:
	Label(IUIScene *scene) : ASceneComponent(scene), _tr(2560.0f, 1440.0f), _scale(0.6f)
	{}

	void draw(Shader &shader) override
	{
		shader.setUniform4f("color", glm::vec4(glm::vec3(1.0f), 1.0f));
		_tr.drawText(_text, _scale, glm::vec3(1.0f, 1.0f, 1.0f), getAnchor());

		shader.setUniform4f("color", glm::vec4(0.0f));
	};

	void setText(std::string const &text) { _text = text; }
	void setScale(float value) { _scale = value; }

private:
	std::string _text;
	TextRenderer _tr;
	float _scale;
};
