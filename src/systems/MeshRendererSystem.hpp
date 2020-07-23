#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/DirectionalLightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "Engine.hpp"
#include "ShaderManager.hpp"
#include <fmt/format.h>
#include <glm/gtx/projection.hpp>
#include "Engine.hpp"
#include <random>
#include "GBuffer.hpp"
#include "TextureAutoBind.hpp"
#include "engine/renderer/Renderer.hpp"
#include "engine/renderer/Shader.hpp"

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	opfor::SharedPtr<opfor::Shader> _billboard;
	opfor::SharedPtr<opfor::Shader> _light;
	opfor::SharedPtr<opfor::Shader> _shadow;
	opfor::Mesh _quad;

	GLuint _ssaoFb;
	GLuint _ssaoBlurFb;
	GLuint _ssaoColorBuf;
	GLuint _ssaoNoiseTex;
	GLuint _ssaoBlurTex;
	opfor::SharedPtr<opfor::Shader> _ssaoShader;
	opfor::SharedPtr<opfor::Shader> _ssaoBlurShader;

	GBuffer _gBuffer;

	GLuint _depthmapFb;
	GLuint _depthCubemap;
	glm::mat4 _shadowProjection;

	Callback<> buildShadowMap;

	static constexpr unsigned int ShadowWidth  = 2048;
	static constexpr unsigned int ShadowHeight = 2048;

	void InitDepthCubemap()
	{
		_shadow = opfor::Shader::Create();
		_shadow->AddVertexShader("shaders/shadow.vs.glsl");
		_shadow->AddFragmentShader("shaders/shadow.fs.glsl");
		_shadow->AddGeometryShader("shaders/shadow.gs.glsl");
		_shadow->Link();

		float aspect = static_cast<float>(ShadowWidth) / static_cast<float>(ShadowHeight);
		float near = 1.0f;
		float far = 1000.0f;
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
	}

	void UnbindShadowMap()
	{
		auto display = opfor::Engine::Get().GetWindow();
		auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

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
		_billboard = opfor::Shader::Create();
		_billboard->AddVertexShader("shaders/billboard.vs.glsl");
		_billboard->AddFragmentShader("shaders/billboard.fs.glsl");
		_billboard->Link();
	}

	void InitSSAO()
	{
		auto display = opfor::Engine::Get().GetWindow();
		auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

		_ssaoShader = opfor::Shader::Create();
		_ssaoShader->AddVertexShader("shaders/ssao.vs.glsl");
		_ssaoShader->AddFragmentShader("shaders/ssao.fs.glsl");
		_ssaoShader->Link();

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

		_ssaoShader->Bind();
		_ssaoShader->SetUniform("gPosition", 0);
		_ssaoShader->SetUniform("gNormal", 1);
		_ssaoShader->SetUniform("texNoise", 2);
		_ssaoShader->Unbind();

		_ssaoBlurShader = opfor::Shader::Create();
		_ssaoBlurShader->AddVertexShader("shaders/ssao.vs.glsl");
		_ssaoBlurShader->AddFragmentShader("shaders/ssaoblur.fs.glsl");
		_ssaoBlurShader->Link();

		_ssaoBlurShader->Bind();
		_ssaoBlurShader->SetUniform("ssaoInput", 0);
		_ssaoBlurShader->Unbind();

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
			float scale = static_cast<float>(i) / 64.0f;
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

		_ssaoShader->Bind();
		_ssaoShader->SetUniform("samples", ssaoKernel, 64);
		_ssaoShader->Unbind();
	}

	void RenderSSAO(PlayerCameraComponent const &camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFb);
		_ssaoShader->Bind();
		_ssaoShader->SetUniform("gPosition", 0);
		_ssaoShader->SetUniform("gNormal", 1);
		_ssaoShader->SetUniform("texNoise", 2);

		glClear(GL_COLOR_BUFFER_BIT);
		// glActiveTexture(GL_TEXTURE0);
			// glBindTexture(GL_TEXTURE_2D, _gBuffer.GetPositionTex());
		// glActiveTexture(GL_TEXTURE1);
		// 	glBindTexture(GL_TEXTURE_2D, _gBuffer.GetNormalTex());
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);

		_ssaoShader->SetUniform("projectionMatrix", camera.projection);
		_ssaoShader->SetUniform("viewMatrix", camera.view);

		opfor::Renderer::Submit(_quad.GetVertexArray());

		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		_ssaoShader->Unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFb);
//
//		_ssaoBlurShader.bind();
//		_ssaoBlurShader.setUniform1i("ssaoInput", 0);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);
//		_quad.Draw();
//		glBindTexture(GL_TEXTURE_2D, 0);
//		_ssaoShader.unbind();

//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderShadowMeshes()
	{
		auto entities = GetEntities<ModelComponent, TransformComponent>();

		for (auto const entity: entities) {

			auto [ model, transform ] = entity->GetAll();

			for (auto const meshId : model.Meshes) {

				auto const *mesh = opfor::Engine::Get().GetMesh(meshId);

				glm::mat4 model(1.0f);
				model = glm::translate(model, transform.position);
				model = glm::scale(model, transform.scale);
				_shadow->SetUniform("modelMatrix", model);

				opfor::Renderer::Submit(reinterpret_cast<opfor::Mesh const*>(mesh)->GetVertexArray());
			}

		}
	}

	void RenderMeshes(PlayerCameraComponent const &camera, TransformComponent const &playerTransform)
	{
		auto models = GetEntities<ModelComponent, TransformComponent>();

		for (auto const &entity : models) {

			auto const [ model, transform ] = entity->GetAll();

			for (auto const meshId : model.Meshes) {

				auto const mesh = opfor::Engine::Get().GetMesh(meshId);
				auto const shaderId = model.Shader;

				auto shaderOpt = ShaderManager::Get().Get(shaderId);

				if (!shaderOpt.has_value()) {
					Logger::Warn("Shader {} does not exist\n", shaderId);
					continue ;
				}

				auto &shader = shaderOpt.value();

				shader->Bind();
				shader->SetUniform("viewProjectionMatrix", camera.viewProjection);
				shader->SetUniform("viewMatrix", camera.view);
				shader->SetUniform("projectionMatrix", camera.projection);
				shader->SetUniform("viewPos", playerTransform.position);

				//UpdateLight(*shader);

				glm::mat4 modelMatrix(1.0f);
				modelMatrix = glm::translate(modelMatrix, transform.position);
				modelMatrix = glm::scale(modelMatrix, transform.scale);
				shader->SetUniform("modelMatrix", modelMatrix);

				std::vector<TextureAutoBind> textureBindings;

				// Bind each texture
				auto meshCast = dynamic_cast<opfor::Mesh*>(mesh);
				if (meshCast != nullptr) {
					if (meshCast->GetPbrMaterial().has_value()) {

						auto material = opfor::Engine::Get().GetPbrMaterial(
							meshCast->GetPbrMaterial().value());

						if (material.has_value()) {
							auto m = material.value();

							shader->SetUniform("material.baseColor", m->BaseColor);
							shader->SetUniform("material.metallicFactor", m->MetallicFactor);
							shader->SetUniform("material.roughnessFactor", m->RoughnessFactor);

							if (m->Albedo.has_value()) {
								auto albedo = m->Albedo.value();
								auto texture = TextureManager::Get().get(albedo);

								shader->SetUniform("material.hasAlbedo", 1);
								textureBindings.push_back(TextureAutoBind(GL_TEXTURE0, GL_TEXTURE_2D, texture));
							}
							else {
								shader->SetUniform("material.hasAlbedo", 0);
							}

							if (m->MetallicRoughness.has_value()) {
								auto metallicRoughness = m->MetallicRoughness.value();
								auto texture = TextureManager::Get().get(metallicRoughness);

								shader->SetUniform("material.hasMetallicRoughness", 1);
								textureBindings.push_back(TextureAutoBind(GL_TEXTURE1, GL_TEXTURE_2D, texture));
							}
							else {
								shader->SetUniform("material.hasMetallicRoughness", 0);
							}

							if (m->Normal.has_value()) {
								auto normal = m->Normal.value();
								auto texture = TextureManager::Get().get(normal);

								textureBindings.push_back(TextureAutoBind(GL_TEXTURE2, GL_TEXTURE_2D, texture));
							}
							else {
								auto const defaultNormal = TextureManager::Get().get("default_normal");
								textureBindings.push_back(TextureAutoBind(GL_TEXTURE2, GL_TEXTURE_2D, defaultNormal));
							}
						}
					}
				}


				opfor::Renderer::Submit(reinterpret_cast<opfor::Mesh const*>(mesh)->GetVertexArray());
				shader->Unbind();
			}

		}
	}

	void RenderSkybox(PlayerCameraComponent const &camera)
	{
		auto skybox = GetEntities<MeshComponent, SkyboxComponent>();

		if (skybox.size() == 0) { return ; }

		auto [ meshComponent, _ ] = skybox[0]->GetAll();
		auto mesh = opfor::Engine::Get().GetMesh(meshComponent.Id);

		auto shader = ShaderManager::Get().Get(meshComponent.Shader).value();

		shader->Bind();
		shader->SetUniform("viewMatrix", glm::mat4(glm::mat3(camera.view)));
		shader->SetUniform("projectionMatrix", camera.projection);

		glDepthMask(GL_FALSE);
		TextureManager::Get().bind("skybox-cubemap", 0);
		opfor::Renderer::Submit(reinterpret_cast<opfor::Mesh const*>(mesh)->GetVertexArray());
		glDepthMask(GL_TRUE);

		shader->Unbind();
	}

	void UpdateLight(opfor::Shader &shader)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0) { return ; }

		shader.SetUniform("pointLightCount", lights.size());

		for (size_t i = 0; i < lights.size(); i++) {
			auto [ light, transform ] = lights[i]->GetAll();

			std::string pointLight = "pointLight[" + std::to_string(i) + "]";

			shader.SetUniform(pointLight + ".position", transform.position);
			shader.SetUniform(pointLight + ".color", light.Color);
			shader.SetUniform(pointLight + ".intensity", light.Intensity);
		}

		auto dirLights = GetEntities<DirectionalLightComponent>();

		shader.SetUniform("directionalLightCount", dirLights.size());

		for (size_t i = 0; i < dirLights.size(); i++) {
			auto [ light ] = dirLights[i]->GetAll();

			std::string directionalLight = "directionalLights[" + std::to_string(i) + "]";

			shader.SetUniform(directionalLight + ".direction", light.Direction);
			shader.SetUniform(directionalLight + ".color", light.Color);
			shader.SetUniform(directionalLight + ".intensity", light.Intensity);
		}
	}

	void RenderLightBillboard(PlayerCameraComponent const &camera)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0 ) { return ; }

		for (auto const &lightEnt : lights) {
			auto [ light, transform ] = lightEnt->GetAll();

			_billboard->Bind();
			_billboard->SetUniform("viewMatrix", camera.view);
			_billboard->SetUniform("viewProjectionMatrix", camera.viewProjection);
			_billboard->SetUniform("projectionMatrix", camera.projection);
			_billboard->SetUniform("particlePosition", transform.position);

			TextureManager::Get().bind("light_bulb_icon", 0);
			opfor::Renderer::Submit(_quad.GetVertexArray());

			_billboard->Unbind();
		}
	}

	void RenderLight(glm::vec3 const &viewPos, float const exposure)
	{
		// Lighting pass
		_light->Bind();
			UpdateLight(*_light);
			_light->SetUniform("gPosition", 0);
			_light->SetUniform("gNormal", 1);
			_light->SetUniform("gAlbedoSpec", 2);
			_light->SetUniform("gSSAO", 3);
			_light->SetUniform("gMetallicRoughness", 5);
			_light->SetUniform("depthMap", 4);
			_light->SetUniform("viewPos", viewPos);
			_light->SetUniform("exposure", exposure);

			// Bind GBuffer Textures

			opfor::Renderer::PushTexture(_gBuffer.GetPositionTex(),   opfor::TextureUnit::Texture0);
			opfor::Renderer::PushTexture(_gBuffer.GetNormalTex(),     opfor::TextureUnit::Texture1);
			opfor::Renderer::PushTexture(_gBuffer.GetAlbedoSpecTex(), opfor::TextureUnit::Texture2);

			glActiveTexture(GL_TEXTURE3);
				//glBindTexture(GL_TEXTURE_2D, _ssaoBlurTex);
				glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);

			glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubemap);

			opfor::Renderer::PushTexture(_gBuffer.GetMetallicRoughnessTex(), opfor::TextureUnit::Texture5);

			opfor::Renderer::Submit(_quad.GetVertexArray());

			// Unbind Textures

			opfor::Renderer::PopTexture(opfor::TextureUnit::Texture5);

			glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);

			opfor::Renderer::PopTexture(opfor::TextureUnit::Texture2);
			opfor::Renderer::PopTexture(opfor::TextureUnit::Texture1);
			opfor::Renderer::PopTexture(opfor::TextureUnit::Texture0);

		_light->Unbind();
	}

	void BakeShadowMap()
	{
		//Logger::Info("Building shadow map\n");

		auto lights = GetEntities<PointLightComponent, TransformComponent>();
		auto players = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (lights.size() == 0) return ;
		if (players.size() == 0) return ;

		auto playerTransform = players[0]->Get<TransformComponent>();
		auto lightPos = lights[0]->Get<TransformComponent>().position;

		std::vector<glm::mat4> shadowTransforms = {
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
			_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)),
		};

		BindShadowMap();

		opfor::Renderer::PushCapability(opfor::RendererCaps::Blend, false);
		opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
			glClear(GL_DEPTH_BUFFER_BIT);

		opfor::Renderer::Shader::Push(_shadow);
		_shadow->Bind();
		_shadow->SetUniform("model", glm::mat4(1.0f));
		_shadow->SetUniform("shadowMatrices", shadowTransforms);
		_shadow->SetUniform("far_plane", 10000.0f);
		_shadow->SetUniform("lightPos", lightPos);

			RenderShadowMeshes();

		opfor::Renderer::Shader::Pop();

		UnbindShadowMap();

		opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);
		opfor::Renderer::PopCapability(opfor::RendererCaps::Blend);
	}

public:
	MeshRendererSystem()
	{
		buildShadowMap = [this] { BakeShadowMap(); };
		opfor::Engine::Get().OnBuildLighting += buildShadowMap;

		InitFramebuffer();
		InitBillboard();
		InitSSAO();
		InitDepthCubemap();

		TextureManager::Get().createTexture("light_bulb_icon", "./img/light_bulb_icon.png", {
			{ GL_TEXTURE_WRAP_R, GL_WRAP_BORDER },
			{ GL_TEXTURE_WRAP_S, GL_WRAP_BORDER },
			{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		}, GL_TEXTURE_2D);

		_light = opfor::Shader::Create();

		_light->AddVertexShader("shaders/light.vs.glsl");
		_light->AddFragmentShader("shaders/light.fs.glsl");
		_light->Link();
	}

	~MeshRendererSystem()
	{
		opfor::Engine::Get().OnBuildLighting -= buildShadowMap;
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto player = GetEntities<PlayerCameraComponent, TransformComponent>();
		//auto display = opfor::Engine::Get().GetWindow();
		//auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

		if (player.size() == 0) { return ; }

		auto [ playerCamera, playerTransform ] = player[0]->GetAll();

		opfor::Renderer::BeginScene();

		opfor::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		opfor::Renderer::Clear();

		BakeShadowMap();

		opfor::Renderer::PushFramebuffer(_gBuffer.GetFramebuffer());
			opfor::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			opfor::Renderer::Clear();

			opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
				RenderMeshes(playerCamera, playerTransform);
			opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);

		opfor::Renderer::PopFramebuffer();

		opfor::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		opfor::Renderer::Clear();

		RenderLight(playerTransform.position, playerCamera.exposure);

		// Copy depth buffer to default framebuffer to enable depth testing with billboard
		// and other shaders
		opfor::Renderer::CopyFramebufferToDefaultFramebuffer(_gBuffer.GetFramebuffer(), opfor::CopyTarget::DepthBufferBit);

		opfor::Renderer::PushCapability(opfor::RendererCaps::Blend, true);
		opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
			RenderSkybox(playerCamera);
//			RenderSSAO(playerCamera);
			RenderLightBillboard(playerCamera);
		opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);
		opfor::Renderer::PopCapability(opfor::RendererCaps::Blend);

		opfor::Renderer::EndScene();
	}
};
