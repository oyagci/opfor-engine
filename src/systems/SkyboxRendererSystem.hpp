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
#include "engine/renderer/Shader.hpp"

using namespace std::placeholders;

class SkyboxRendererSystem : public ecs::ComponentSystem
{
private:
	opfor::UniquePtr<opfor::Shader> _shader;

public:
	SkyboxRendererSystem()
	{
		_shader = opfor::Shader::Create();
		_shader->AddVertexShader("shaders/cubemap.vs.glsl");
		_shader->AddFragmentShader("shaders/cubemap.fs.glsl");
		_shader->Link();
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

		_shader->Bind();
		_shader->SetUniform("viewMatrix", glm::mat4(glm::mat3(cameraData.view)));
		_shader->SetUniform("projectionMatrix", cameraData.projection);

		glDepthMask(GL_FALSE);
		TextureManager::Get().bind("skybox-cubemap", 0);
		mesh->Draw();
		glDepthMask(GL_TRUE);

		_shader->Unbind();
	}
};
