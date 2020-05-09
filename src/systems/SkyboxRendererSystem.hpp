#pragma once

#include "ecs/System.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "lazy.hpp"

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
		auto skybox = GetEntities<SkyboxComponent>();

		if (skybox.size() == 0) { return ; }
		if (camera.size() == 0) { return ; }

		auto cameraData = camera[0]->GetComponentData<PlayerCameraComponent>();
		auto skyData = skybox[0]->GetComponentData<SkyboxComponent>();

		_shader.bind();
		_shader.setUniform4x4f("viewMatrix", glm::mat4(glm::mat3(cameraData.view)));
		_shader.setUniform4x4f("projectionMatrix", cameraData.projection);

		glDepthMask(GL_FALSE);
		glBindVertexArray(skyData.vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyData.texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);

		_shader.unbind();
	}
};
