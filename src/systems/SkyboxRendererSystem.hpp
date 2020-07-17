#pragma once

#include "ecs/System.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "lazy.hpp"
#include "ShaderManager.hpp"
#include "components/MeshComponent.hpp"
#include "Engine.hpp"
#include "TextureManager.hpp"
#include <fmt/format.h>

using namespace std::placeholders;

class SkyboxRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _shader;

public:
	SkyboxRendererSystem()
	{
		_shader.addVertexShader("shaders/cubemap.vs.glsl")
			.addFragmentShader("shaders/cubemap.fs.glsl")
			.link();
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto camera = GetEntities<PlayerCameraComponent>();
		auto skybox = GetEntities<MeshComponent, SkyboxComponent>();

		if (skybox.size() == 0) { return ; }
		if (camera.size() == 0) { return ; }

		auto cameraData = camera[0]->Get<PlayerCameraComponent>();
		auto [ meshComponent, _ ] = skybox[0]->GetAll();
		auto mesh = opfor::Engine::Get().GetMesh(meshComponent.Id);

		_shader.bind();
		_shader.setUniform4x4f("viewMatrix", glm::mat4(glm::mat3(cameraData.view)));
		_shader.setUniform4x4f("projectionMatrix", cameraData.projection);

		glDepthMask(GL_FALSE);
		TextureManager::Get().bind("skybox-cubemap", 0);
		mesh->Draw();
		glDepthMask(GL_TRUE);

		_shader.unbind();
	}
};
