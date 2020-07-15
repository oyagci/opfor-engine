#pragma once

#include "ecs/System.hpp"
#include "ui/TextRenderer.hpp"
#include "Engine.hpp"
#include "components/TextComponent.hpp"

class TextRendererSystem : public ecs::ComponentSystem
{
	TextRenderer _TextRenderer;
	lazy::graphics::Shader _Shader;

public:
	TextRendererSystem() : _TextRenderer(engine::Engine::Instance().GetDisplay()->getWidth(),
										 engine::Engine::Instance().GetDisplay()->getHeight())
	{
		_Shader.addVertexShader("shaders/ui.vs.glsl")
			.addFragmentShader("shaders/ui.fs.glsl")
			.link();

		auto screenSize = engine::Engine::Instance().GetDisplay()->getScreenSize();

		_Shader.bind();
		_Shader.setUniform4x4f("projectionMatrix", glm::ortho(0.0f, screenSize.x, 0.0f, screenSize.y));
		_Shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
		_Shader.unbind();
	}

	void OnUpdate(float) override
	{
		auto textEntities = GetEntities<TextComponent>();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		_Shader.bind();

		for (auto const &ent : textEntities) {
			auto const &text = ent->Get<TextComponent>();
			_Shader.setUniform4f("color", glm::vec4(text.Color.x, text.Color.y, text.Color.z, 1.0f));
			_TextRenderer.drawText(text.Text, text.Scale, text.Color, text.Anchor);
		}

		_Shader.unbind();

		glDisable(GL_BLEND);
	}
};
