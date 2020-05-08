#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"

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
		auto meshes = GetEntities<MeshComponent>();
		auto player = GetEntities<PlayerCameraComponent>();

		if (player.size() == 0) {
			return ;
		}

		auto playerCamera = player[0]->GetComponentData<PlayerCameraComponent>();

		_shader.bind();
		_shader.setUniform4x4f("viewProjectionMatrix", playerCamera.viewProjection);
		_shader.setUniform4x4f("modelMatrix", playerCamera.model);
		_shader.setUniform4x4f("viewMatrix", playerCamera.view);
		_shader.setUniform4x4f("projectionMatrix", playerCamera.projection);

		for (auto m : meshes) {
			auto &data = m->GetComponentData<MeshComponent>();

			// Bind each texture
			size_t currentTexture = 0;
			for (auto &t : data.textures) {
				TextureManager::instance().bind(t, currentTexture);
				currentTexture++;
			}

			data.mesh.draw();
		}

		_shader.unbind();
	}
};
