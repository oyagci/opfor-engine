#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "Engine.hpp"
#include "Framebuffer.hpp"

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _shader;
	lazy::graphics::Shader _fbShader;

	engine::Framebuffer _framebuffer;
	lazy::graphics::Mesh _quad;

	void initQuad()
	{
		std::array<glm::vec3, 4> pos = {
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f,  1.0f, 0.0f),
			glm::vec3(-1.0f,  1.0f, 0.0f),
		};
		std::array<glm::vec2, 4> tex = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
		};

		for (auto p : pos) {
			_quad.addPosition(p);
		}
		for (auto t : tex) {
			_quad.addUv(t);
		}
		_quad.addTriangle(glm::uvec3(0, 1, 2));
		_quad.addTriangle(glm::uvec3(0, 2, 3));
		_quad.build();

	}

	void initFramebuffer()
	{
		initQuad();

		_framebuffer.AttachColorBuffer();
		_framebuffer.AttachDepthBuffer();

		_fbShader.addVertexShader("shaders/fb.vs.glsl")
			.addFragmentShader("shaders/fb.fs.glsl")
			.link();

		_fbShader.bind();
		_fbShader.setUniform1i("screen_texture", 0);
		_fbShader.unbind();

		assert(_fbShader.isValid());
	}

public:
	MeshRendererSystem() : _framebuffer(engine::Framebuffer_Type::RW)
	{
		_shader.addVertexShader("./shaders/basic.vs.glsl")
			.addFragmentShader("./shaders/basic.fs.glsl")
			.link();

		if (!_shader.isValid()) {
			std::cout << "Shader Err" << std::endl;
		}

		initFramebuffer();

		assert(_framebuffer.IsComplete());
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto meshes = GetEntities<MeshComponent, TransformComponent>();
		auto player = GetEntities<PlayerCameraComponent>();

		if (player.size() == 0) {
			return ;
		}

		auto playerCamera = player[0]->Get<PlayerCameraComponent>();

		{
			_framebuffer.Bind();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
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
			_framebuffer.Unbind();
		}

		{
			glClearColor(1.0f, 0.0, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _framebuffer.GetColorTexture());
			_fbShader.bind();
			_quad.draw();
			_fbShader.unbind();
		}
	}
};
