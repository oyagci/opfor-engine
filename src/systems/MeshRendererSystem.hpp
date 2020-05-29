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
#include <random>
#include <atomic>

class GBuffer
{
private:
	GLuint _gBuffer;
	GLuint _gPosition;
	GLuint _gNormal;
	GLuint _gAlbedoSpec;
	GLuint _gDepth;

	void Init()
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
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
	GBuffer()
	{
		Init();
	}

	~GBuffer()
	{
		glDeleteFramebuffers(1, &_gBuffer);
		glDeleteTextures(1, &_gPosition);
		glDeleteTextures(1, &_gNormal);
		glDeleteTextures(1, &_gAlbedoSpec);
		glDeleteTextures(1, &_gDepth);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint GetFramebufferId() { return _gBuffer; }
	GLuint GetPositionTex() { return _gPosition; }
	GLuint GetNormalTex() { return _gNormal; }
	GLuint GetAlbedoSpecTex() { return _gAlbedoSpec; }
	GLuint GetDepthTex() { return _gDepth; }
};

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _billboard;
	lazy::graphics::Shader _light;
	lazy::graphics::Shader _shadow;
	lazy::graphics::Mesh _quad;

	GLuint _ssaoFb;
	GLuint _ssaoBlurFb;
	GLuint _ssaoColorBuf;
	GLuint _ssaoNoiseTex;
	GLuint _ssaoBlurTex;
	lazy::graphics::Shader _ssaoShader;
	lazy::graphics::Shader _ssaoBlurShader;

	GBuffer _gBuffer;

	GLuint _depthmapFb;
	GLuint _depthCubemap;
	glm::mat4 _shadowProjection;

	static constexpr unsigned int ShadowWidth  = 2048;
	static constexpr unsigned int ShadowHeight = 2048;

	void InitDepthCubemap()
	{
		_shadow.addVertexShader("shaders/shadow.vs.glsl")
			.addFragmentShader("shaders/shadow.fs.glsl")
			.addGeometryShader("shaders/shadow.gs.glsl")
			.link();
		assert(_shadow.isValid());

		_shadow.bind();

		float aspect = static_cast<float>(ShadowWidth) / static_cast<float>(ShadowHeight);
		float near = 1.0f;
		float far = 10000.0f;
		_shadowProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);

		glGenFramebuffers(1, &_depthmapFb);
		glBindFramebuffer(GL_FRAMEBUFFER, _depthmapFb);

		glGenTextures(1, &_depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubemap);
		for (size_t face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindShadowMap()
	{
		glViewport(0, 0, ShadowWidth, ShadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, _depthmapFb);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void UnbindShadowMap()
	{
		auto display = engine::Engine::Instance().GetDisplay();
		auto [ width, height ] = std::tuple(display->getWidth(), display->getHeight());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void InitQuad()
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

	void InitFramebuffer()
	{
		InitQuad();
	}

	void InitBillboard()
	{
		_billboard.addVertexShader("shaders/billboard.vs.glsl")
			.addFragmentShader("shaders/billboard.fs.glsl")
			.link();
	}

	void InitSSAO()
	{
		auto display = engine::Engine::Instance().GetDisplay();
		auto [ width, height ] = std::tuple(display->getWidth(), display->getHeight());

		_ssaoShader.addVertexShader("shaders/ssao.vs.glsl")
			.addFragmentShader("shaders/ssao.fs.glsl")
			.link();
		assert(_ssaoShader.isValid());

		glGenFramebuffers(1, &_ssaoFb);
		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFb);

		glGenTextures(1, &_ssaoColorBuf);
		glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuf, 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GenSSAOKernel();

		_ssaoShader.bind();
		_ssaoShader.setUniform1i("gPosition", 0);
		_ssaoShader.setUniform1i("gNormal", 1);
		_ssaoShader.setUniform1i("texNoise", 2);
		_ssaoShader.unbind();

		_ssaoBlurShader.addVertexShader("shaders/ssao.vs.glsl")
			.addFragmentShader("shaders/ssaoblur.fs.glsl")
			.link();

		glGenFramebuffers(1, &_ssaoBlurFb);
		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFb);

		glGenTextures(1, &_ssaoBlurFb);
		glBindTexture(GL_TEXTURE_2D, _ssaoBlurFb);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuf, 0);
	}

	void GenSSAOKernel()
	{
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		std::vector<glm::vec3> ssaoKernel;

		auto lerp = [] (float a, float b, float f) -> float {
			return a + f * (b - a);
		};

		for (size_t i = 0; i < 64; i++) {
			glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f,
							 randomFloats(generator) * 2.0f - 1.0f,
							 randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);

			// Make distribution closer to origin
			float scale = (float)i / 64.0f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;

			ssaoKernel.push_back(sample);
		}

		std::array<glm::vec3, 64> ssaoNoise;
		for (size_t i = 0; i < 64; i++) {
			glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f,
							randomFloats(generator) * 2.0f - 1.0f,
							0.0f);
			ssaoNoise[i] = noise;
		}

		glGenTextures(1, &_ssaoNoiseTex);
		glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		_ssaoShader.bind();
		GLuint sampleLoc = _ssaoShader.getUniformLocation("samples");
		glUniform3fv(sampleLoc, 64, &ssaoKernel[0][0]);
		_ssaoShader.unbind();
	}

	void RenderSSAO(PlayerCameraComponent &camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFb);
		_ssaoShader.bind();

		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _gBuffer.GetPositionTex());
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _gBuffer.GetNormalTex());
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);

		_ssaoShader.setUniform4x4f("projectionMatrix", camera.projection);
		_quad.draw();
		_ssaoShader.unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFb);
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);
		_ssaoBlurShader.bind();
		_quad.draw();
		_ssaoShader.unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderShadowMeshes(TransformComponent &playerTransform)
	{
		auto meshes = GetEntities<MeshComponent, TransformComponent>();

		for (auto m : meshes) {
			auto [ meshId, shaderId ] = m->Get<MeshComponent>();
			auto data = engine::Engine::Instance().GetMesh(meshId);
			auto &transform = m->Get<TransformComponent>();

			glm::mat4 model(1.0f);
			model = glm::scale(model, transform.scale);
			model = glm::translate(model, transform.position);
			_shadow.setUniform4x4f("modelMatrix", model);

			data->draw();

		}
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

	void RenderLight(glm::vec3 const &viewPos)
	{
		// Lighting pass
		_light.bind();
			UpdateLight(_light);
			_light.setUniform1i("gPosition", 0);
			_light.setUniform1i("gNormal", 1);
			_light.setUniform1i("gAlbedoSpec", 2);
			_light.setUniform1i("gSSAO", 3);
			_light.setUniform1i("depthMap", 4);
			_light.setUniform3f("viewPos", viewPos);

			// Bind GBuffer Textures
			glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, _gBuffer.GetPositionTex());
			glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, _gBuffer.GetNormalTex());
			glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, _gBuffer.GetAlbedoSpecTex());
			glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, _ssaoBlurTex);
			glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubemap);

			_quad.draw();
		_light.unbind();

		// Unbind Textures
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
	}

public:
	MeshRendererSystem()
	{
		InitFramebuffer();
		InitBillboard();
		InitSSAO();
		InitDepthCubemap();

		TextureManager::instance().createTexture("light_bulb_icon", "./img/light_bulb_icon.png", {
			{ GL_TEXTURE_WRAP_R, GL_WRAP_BORDER },
			{ GL_TEXTURE_WRAP_S, GL_WRAP_BORDER },
			{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		}, GL_TEXTURE_2D);

		_light.addVertexShader("shaders/light.vs.glsl")
			.addFragmentShader("shaders/light.fs.glsl")
			.link();
//		assert(_light.isValid());
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto player = GetEntities<PlayerCameraComponent, TransformComponent>();
		auto display = engine::Engine::Instance().GetDisplay();
		auto [ width, height ] = std::tuple(display->getWidth(), display->getHeight());
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (player.size() == 0) { return ; }

		auto playerCamera = player[0]->Get<PlayerCameraComponent>();
		auto playerTransform = player[0]->Get<TransformComponent>();

		glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
		BindShadowMap();
		{
			auto l = lights[0];
			auto [ pl, tc ] = l->GetAll();

			auto lightPos = tc.position;

			std::array<glm::mat4, 6> shadowTransforms = {
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
				_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)),
			};

			_shadow.bind();
			_shadow.setUniform4x4f("model", glm::mat4(1.0f));
			glUniformMatrix4fv(_shadow.getUniformLocation("shadowMatrices"), 6, GL_FALSE, (float *)shadowTransforms.data());
			_shadow.setUniform1f("far_plane", 10000.0f);
			_shadow.setUniform3f("lightPos", lightPos);
			RenderShadowMeshes(playerTransform);
			_shadow.unbind();
		}
		UnbindShadowMap();

		_gBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		RenderMeshes(playerCamera, playerTransform);
		_gBuffer.Unbind();

		glClearColor(0.0f, 0.0, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		RenderLight(playerTransform.position);

		// Copy depth buffer to default framebuffer to enable depth testing with billboard
		// and other shaders
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _gBuffer.GetFramebufferId());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		RenderSkybox(playerCamera);
		RenderSSAO(playerCamera);
		RenderLightBillboard(playerCamera);
	}
};
