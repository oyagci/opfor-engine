#pragma once

#include "ecs/System.hpp"
#include "MeshComponent.hpp"
#include "lazy.hpp"

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _shader;

	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewProjection;

public:
	MeshRendererSystem()
	{
		_shader.addVertexShader("./shaders/basic.vs.glsl")
			.addFragmentShader("./shaders/basic.fs.glsl")
			.link();

		if (!_shader.isValid()) {
			std::cout << "Shader Err" << std::endl;
		}

		projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
		view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0, 0.0, -10.0f));
		model = glm::mat4(1.0f);
		viewProjection = projection * view;

		_shader.bind();
		_shader.setUniform4x4f("viewProjectionMatrix", viewProjection);
		_shader.setUniform4x4f("modelMatrix", model);
		_shader.setUniform4x4f("viewMatrix", view);
		_shader.setUniform4x4f("projectionMatrix", projection);
		_shader.unbind();
	}
	void OnUpdate(float __unused deltaTime) override
	{
		auto meshes = GetEntities<MeshComponent>();

		_shader.bind();

		for (auto m : meshes) {
			auto &data = m->GetComponentData<MeshComponent>();

			data.mesh.draw();
		}

		_shader.unbind();
	}
};
