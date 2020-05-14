#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "Engine.hpp"

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _shader;

public:
	MeshRendererSystem()
	{
		_shader.addVertexShader("./shaders/basic.vs.glsl")
			.addFragmentShader("./shaders/basic.fs.glsl")
			.link();

		if (!_shader.isValid()) {
			std::cout << "Shader Err" << std::endl;
		}
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto meshes = GetEntities<MeshComponent, TransformComponent>();
		auto player = GetEntities<PlayerCameraComponent>();

		if (player.size() == 0) {
			return ;
		}

		auto playerCamera = player[0]->Get<PlayerCameraComponent>();

		_shader.bind();
		_shader.setUniform4x4f("viewProjectionMatrix", playerCamera.viewProjection);
		_shader.setUniform4x4f("viewMatrix", playerCamera.view);
		_shader.setUniform4x4f("projectionMatrix", playerCamera.projection);

		for (auto m : meshes) {
			auto data = engine::Engine::Instance().GetMesh(m->Get<MeshComponent>().Id);
			auto &transform = m->Get<TransformComponent>();

			glm::mat4 model(1.0f);
			model = glm::scale(model, transform.scale);
			model = glm::translate(model, transform.position);
			_shader.setUniform4x4f("modelMatrix", model);

			// Bind each texture
			size_t currentTexture = 0;
			for (auto &t : data->getTextures()) {
				TextureManager::instance().bind(t, currentTexture);
				currentTexture++;
			}

			data->draw();
		}

		_shader.unbind();
	}
};
