#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "Engine.hpp"
#include "Framebuffer.hpp"
#include "ShaderManager.hpp"
#include <fmt/format.h>
#include <glm/gtx/projection.hpp>
#include "Engine.hpp"

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _fbShader;
	lazy::graphics::Shader _billboard;
	lazy::graphics::Shader _light;

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

	void initBillboard()
	{
		_billboard.addVertexShader("shaders/billboard.vs.glsl")
			.addFragmentShader("shaders/billboard.fs.glsl")
			.link();
	}

	GLuint _gBuffer;
	GLuint _gPosition;
	GLuint _gNormal;
	GLuint _gAlbedoSpec;
	GLuint _gDepth;

	void initGbuffer()
	{

		auto display = engine::Engine::Instance().GetDisplay();
		auto [ width, height ] = std::tuple(display->getWidth(), display->getHeight());

		glGenFramebuffers(1, &_gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

		glGenTextures(1, &_gPosition);
		glBindTexture(GL_TEXTURE_2D, _gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);

		glGenTextures(1, &_gNormal);
		glBindTexture(GL_TEXTURE_2D, _gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

		glGenTextures(1, &_gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &_gDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, _gDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _gDepth);

		GLuint st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assert(st == GL_FRAMEBUFFER_COMPLETE);

		std::array<GLuint, 3> attachments = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		glDrawBuffers(3, attachments.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

public:
	MeshRendererSystem() : _framebuffer(engine::Framebuffer_Type::RW)
	{
		initFramebuffer();
		initBillboard();
		initGbuffer();

		TextureManager::instance().createTexture("light_bulb_icon", "./img/light_bulb_icon.png", {
			{ GL_TEXTURE_WRAP_R, GL_WRAP_BORDER },
			{ GL_TEXTURE_WRAP_S, GL_WRAP_BORDER },
			{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		}, GL_TEXTURE_2D);

		assert(_framebuffer.IsComplete());

		_light.addVertexShader("shaders/light.vs.glsl")
			.addFragmentShader("shaders/light.fs.glsl")
			.link();
		assert(_light.isValid());
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto player = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (player.size() == 0) { return ; }

		auto playerCamera = player[0]->Get<PlayerCameraComponent>();
		auto playerTransform = player[0]->Get<TransformComponent>();

		glDisable(GL_BLEND);

		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		RenderMeshes(playerCamera, playerTransform);
		RenderSkybox(playerCamera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		_light.bind();

		UpdateLight(_light);
		_light.setUniform1i("gPosition", 0);
		_light.setUniform1i("gNormal", 1);
		_light.setUniform1i("gAlbedoSpec", 2);
		_light.setUniform3f("viewPos", playerTransform.position);

		// Bind GBuffer Textures
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _gPosition);
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _gNormal);
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec);

		_quad.draw();
		_light.unbind();

		// Unbind Textures
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);

		glEnable(GL_BLEND);
		RenderLightBillboard(playerCamera);
	}

	void RenderMeshes(PlayerCameraComponent &camera, TransformComponent &playerTransform)
	{
		auto meshes = GetEntities<MeshComponent, TransformComponent>();

		for (auto m : meshes) {
			auto [ meshId, shaderId ] = m->Get<MeshComponent>();
			auto data = engine::Engine::Instance().GetMesh(meshId);
			auto &transform = m->Get<TransformComponent>();

			auto shader = ShaderManager::instance().Get(shaderId).value();
			shader->bind();
			shader->setUniform4x4f("viewProjectionMatrix", camera.viewProjection);
			shader->setUniform4x4f("viewMatrix", camera.view);
			shader->setUniform4x4f("projectionMatrix", camera.projection);
			shader->setUniform3f("viewPos", playerTransform.position);

			UpdateLight(*shader);

			glm::mat4 model(1.0f);
			model = glm::scale(model, transform.scale);
			model = glm::translate(model, transform.position);
			shader->setUniform4x4f("modelMatrix", model);

			TextureManager::instance().bind("default_normal", 2);

			// Bind each texture
			size_t currentTexture = 0;
			for (auto &t : data->getTextures()) {
				switch (t.type) {
				case assimp::TextureType::TT_Diffuse:
					TextureManager::instance().bind(t.name, 0);
					break ;
				case assimp::TextureType::TT_Specular:
					TextureManager::instance().bind(t.name, 1);
					break ;
				case assimp::TextureType::TT_Normal:
					TextureManager::instance().bind(t.name, 2);
					break ;
				default:
					fmt::print(stderr, "[WARNING] Texture type ({}) not handled\n",
						t.type);
					break ;
				}
				currentTexture++;
			}

			data->draw();

			// Unbind Textures
			glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
			shader->unbind();
		}
	}

	void RenderSkybox(PlayerCameraComponent &camera)
	{
		auto skybox = GetEntities<MeshComponent, SkyboxComponent>();

		if (skybox.size() == 0) { return ; }

		auto [ meshComponent, _ ] = skybox[0]->GetAll();
		auto mesh = engine::Engine::Instance().GetMesh(meshComponent.Id);

		auto shader = ShaderManager::instance().Get(meshComponent.Shader).value();

		shader->bind();
		shader->setUniform4x4f("viewMatrix", glm::mat4(glm::mat3(camera.view)));
		shader->setUniform4x4f("projectionMatrix", camera.projection);

		glDepthMask(GL_FALSE);
		TextureManager::instance().bind("skybox-cubemap", 0);
		mesh->draw();
		glDepthMask(GL_TRUE);

		shader->unbind();
	}

	void UpdateLight(lazy::graphics::Shader &shader)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0) { return ; }

		shader.setUniform1i("pointLightCount", lights.size());

		for (size_t i = 0; i < lights.size(); i++) {
			auto [ light, transform ] = lights[i]->GetAll();

			shader.setUniform3f("pointLight[" + std::to_string(i) + "].position", transform.position);
			shader.setUniform3f("pointLight[" + std::to_string(i) + "].ambient", light.Color);
			shader.setUniform3f("pointLight[" + std::to_string(i) + "].diffuse", light.Color);
			shader.setUniform3f("pointLight[" + std::to_string(i) + "].specular", light.Color);

			shader.setUniform1f("pointLight[" + std::to_string(i) + "].constant", light.constant);
			shader.setUniform1f("pointLight[" + std::to_string(i) + "].linear", light.linear);
			shader.setUniform1f("pointLight[" + std::to_string(i) + "].quadratic", light.quadratic);
		}
	}

	void RenderLightBillboard(PlayerCameraComponent &camera)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0 ) { return ; }

		for (auto const &lightEnt : lights) {
			auto [ light, transform ] = lightEnt->GetAll();

			_billboard.bind();
			_billboard.setUniform4x4f("viewMatrix", camera.view);
			_billboard.setUniform4x4f("viewProjectionMatrix", camera.viewProjection);
			_billboard.setUniform4x4f("projectionMatrix", camera.projection);
			_billboard.setUniform3f("particlePosition", transform.position);

			TextureManager::instance().bind("light_bulb_icon", 0);
			_quad.draw();

			_billboard.unbind();
		}
	}
};
