#include "MeshRenderer.hpp"

#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include "opfor/core/Application.hpp"
#include "opfor/renderer/TextureManager.hpp"
#include "opfor/renderer/ShaderManager.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/core/ImageLoader.hpp"

#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/DirectionalLightComponent.hpp"
#include "components/ModelComponent.hpp"

namespace opfor {

MeshRenderer::MeshRenderer()
{
	InitQuad();
	InitBillboard();
	// InitSSAO();
	InitDepthCubemap();

	auto texture = TextureManager::Get().Create2D("light_bulb_icon");
	auto img = opfor::ImageLoader::Load("./img/light_bulb_icon.png");

	opfor::TextureParameterList texParams = {
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::LinearMipmapLinear },
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::ClampToEdge },
	};

	texture->SetDataType(opfor::DataType::UnsignedByte);
	texture->SetHasAlpha(img.nchannel == 4);
	texture->SetIsSRGB(true);
	texture->SetInputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	texture->SetOutputFormat(img.nchannel == 4 ? opfor::DataFormat::RGBA : opfor::DataFormat::RGB);
	texture->SetSize(img.width, img.height);
	texture->SetTextureParameters(texParams);
	texture->SetGenerateMipmap(true);
	texture->SetData(img.data.get());
	texture->Build();

	_light = opfor::Shader::Create("shaders/light.glsl");

	_light->Bind();
	_light->SetUniform("gPosition", 0);
	_light->SetUniform("gNormal", 1);
	_light->SetUniform("gAlbedoSpec", 2);
	_light->SetUniform("gSSAO", 3);
	_light->SetUniform("depthMap", 4);
	_light->SetUniform("gMetallicRoughness", 5);
	_light->SetUniform("irradianceMap", 6);
	_light->SetUniform("brdfLUT", 7);
	_light->SetUniform("prefilterMap", 8);
	_light->Unbind();
}

void MeshRenderer::InitDepthCubemap()
{
	_shadow = opfor::Shader::Create("shaders/shadow.glsl");

	float aspect = static_cast<float>(ShadowWidth) / static_cast<float>(ShadowHeight);
	float near = 1.0f;
	float far = 1000.0f;
	_shadowProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);

	_depthCubemap = opfor::TextureCubemap::Create();

	_depthCubemap->SetTextureParameters({
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Nearest },
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Nearest },
		{ opfor::TextureParameterType::WrapR,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::ClampToEdge },
	});
	_depthCubemap->SetInputFormat(opfor::DataFormat::Depth);
	_depthCubemap->SetOutputFormat(opfor::DataFormat::Depth);
	_depthCubemap->SetDataType(opfor::DataType::Float);
	_depthCubemap->SetSize(ShadowWidth, ShadowHeight);

	_depthCubemap->Bind(opfor::TextureUnit::Texture0);

	constexpr std::array<opfor::CubemapFace, 6> faces {
		opfor::CubemapFace::PositiveX, opfor::CubemapFace::NegativeX,
		opfor::CubemapFace::PositiveY, opfor::CubemapFace::NegativeY,
		opfor::CubemapFace::PositiveZ, opfor::CubemapFace::NegativeZ,
	};

	_depthCubemap->Bind(opfor::TextureUnit::Texture0);

	for (size_t i = 0; i < faces.size(); i++) {
		opfor::ImageLoader::Image img{};

		img.width = ShadowWidth;
		img.height = ShadowHeight;

		_depthCubemap->SetFaceData(faces[i], std::move(img));
	}

	_depthCubemap->Build();

	_depthmap = opfor::Framebuffer::Create();
	_depthmap->Bind();
	_depthmap->AttachTexture(_depthCubemap, opfor::FramebufferAttachment::DepthAttachment);
	_depthmap->Unbind();
}

void MeshRenderer::InitQuad()
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

void MeshRenderer::InitBillboard()
{
	_billboard = opfor::Shader::Create("shaders/billboard.glsl");
}

void MeshRenderer::RenderShadowMeshes()
{
	auto entities = opfor::Application::Get().GetEntities<ModelComponent, TransformComponent>();

	for (auto const entity: entities) {

		auto [ model, transform ] = entity->GetAll();

		for (auto const meshId : model.Meshes) {

			auto const *mesh = opfor::Application::Get().GetMesh(meshId);

			glm::mat4 model(1.0f);
			model = glm::translate(model, transform.position);
			model = glm::scale(model, transform.scale);
			opfor::Renderer::Shader::SetUniform("modelMatrix", model);

			opfor::Renderer::Submit(reinterpret_cast<opfor::Mesh const*>(mesh)->GetVertexArray());
		}

	}
}

void MeshRenderer::RenderMeshes(PlayerCameraComponent const &camera, TransformComponent const &playerTransform)
{
	auto models = opfor::Application::Get().GetEntities<ModelComponent, TransformComponent>();

	for (auto const &entity : models) {

		auto const [ model, transform ] = entity->GetAll();

		for (auto const meshId : model.Meshes) {

			auto const mesh = opfor::Application::Get().GetMesh(meshId);
			auto const shaderId = model.Shader;

			auto shaderOpt = ShaderManager::Get().Get(shaderId);

			if (!shaderOpt.has_value()) {
				Logger::Warn("Shader {} does not exist\n", shaderId);
				continue ;
			}

			auto &shader = shaderOpt.value();

			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, transform.position);
			modelMatrix = glm::scale(modelMatrix, transform.scale);

			opfor::Renderer::Shader::Push(shader);
			opfor::Renderer::Shader::SetUniform("viewProjectionMatrix", camera.viewProjection);
			opfor::Renderer::Shader::SetUniform("viewMatrix", camera.view);
			opfor::Renderer::Shader::SetUniform("projectionMatrix", camera.projection);
			opfor::Renderer::Shader::SetUniform("viewPos", playerTransform.position);
			opfor::Renderer::Shader::SetUniform("modelMatrix", modelMatrix);

			bool popAlbedo = false;
			bool popMetallic = false;
			bool popNormal = false;

			// Bind each texture
			auto meshCast = dynamic_cast<opfor::Mesh*>(mesh);
			if (meshCast != nullptr) {
				if (meshCast->GetPbrMaterial().has_value()) {

					auto material = opfor::Application::Get().GetPbrMaterial(
						meshCast->GetPbrMaterial().value());

					if (material.has_value()) {
						auto m = material.value();

						opfor::Renderer::Shader::SetUniform("material.baseColor", m->BaseColor);
						opfor::Renderer::Shader::SetUniform("material.metallicFactor", m->MetallicFactor);
						opfor::Renderer::Shader::SetUniform("material.roughnessFactor", m->RoughnessFactor);

						if (m->Albedo.has_value()) {
							auto albedo = m->Albedo.value();
							auto texture = TextureManager::Get().Get(albedo);

							popAlbedo = true;

							opfor::Renderer::Shader::SetUniform("material.hasAlbedo", 1);
							opfor::Renderer::PushTexture(texture, opfor::TextureUnit::Texture0);
						}
						else {
							opfor::Renderer::Shader::SetUniform("material.hasAlbedo", 0);
						}

						if (m->MetallicRoughness.has_value()) {
							auto metallicRoughness = m->MetallicRoughness.value();
							auto texture = TextureManager::Get().Get(metallicRoughness);

							popMetallic = true;

							opfor::Renderer::Shader::SetUniform("material.hasMetallicRoughness", 1);
							opfor::Renderer::PushTexture(texture, opfor::TextureUnit::Texture1);
						}
						else {
							opfor::Renderer::Shader::SetUniform("material.hasMetallicRoughness", 0);
						}

						if (m->Normal.has_value()) {
							auto normal = m->Normal.value();
							auto texture = TextureManager::Get().Get(normal);

							popNormal = true;

							opfor::Renderer::PushTexture(texture, opfor::TextureUnit::Texture2);
						}
						else {
							auto const defaultNormal = TextureManager::Get().Get("default_normal");
							opfor::Renderer::PushTexture(defaultNormal, opfor::TextureUnit::Texture2);

							popNormal = true;
						}
					}
				}
			}

			opfor::Renderer::Submit(reinterpret_cast<opfor::Mesh const*>(mesh)->GetVertexArray());

			popAlbedo   ? opfor::Renderer::PopTexture(opfor::TextureUnit::Texture0) : (void)0;
			popMetallic ? opfor::Renderer::PopTexture(opfor::TextureUnit::Texture1) : (void)0;
			popNormal   ? opfor::Renderer::PopTexture(opfor::TextureUnit::Texture2) : (void)0;

			opfor::Renderer::Shader::Pop();
		}

	}
}

void MeshRenderer::UpdateLight()
{
	auto lights = opfor::Application::Get().GetEntities<PointLightComponent, TransformComponent>();

	if (lights.size() == 0) { return ; }

	opfor::Renderer::Shader::SetUniform("pointLightCount", lights.size());

	for (size_t i = 0; i < lights.size(); i++) {
		auto [ light, transform ] = lights[i]->GetAll();

		std::string pointLight = "pointLight[" + std::to_string(i) + "]";

		opfor::Renderer::Shader::SetUniform(pointLight + ".position", transform.position);
		opfor::Renderer::Shader::SetUniform(pointLight + ".color", light.Color);
		opfor::Renderer::Shader::SetUniform(pointLight + ".intensity", light.Intensity);
	}

	auto dirLights = opfor::Application::Get().GetEntities<DirectionalLightComponent>();

	opfor::Renderer::Shader::SetUniform("directionalLightCount", dirLights.size());

	for (size_t i = 0; i < dirLights.size(); i++) {
		auto [ light ] = dirLights[i]->GetAll();

		std::string directionalLight = "directionalLights[" + std::to_string(i) + "]";

		opfor::Renderer::Shader::SetUniform(directionalLight + ".direction", light.Direction);
		opfor::Renderer::Shader::SetUniform(directionalLight + ".color", light.Color);
		opfor::Renderer::Shader::SetUniform(directionalLight + ".intensity", light.Intensity);
	}
}

void MeshRenderer::RenderLightBillboard(PlayerCameraComponent const &camera)
{
	auto lights = opfor::Application::Get().GetEntities<PointLightComponent, TransformComponent>();

	if (lights.size() == 0 ) { return ; }

	for (auto const &lightEnt : lights) {
		auto [ light, transform ] = lightEnt->GetAll();

		opfor::Renderer::Shader::Push(_billboard);
		opfor::Renderer::Shader::SetUniform("viewMatrix", camera.view);
		opfor::Renderer::Shader::SetUniform("viewProjectionMatrix", camera.viewProjection);
		opfor::Renderer::Shader::SetUniform("projectionMatrix", camera.projection);
		opfor::Renderer::Shader::SetUniform("particlePosition", transform.position);

		opfor::Renderer::PushTexture(TextureManager::Get().Get("light_bulb_icon"), opfor::TextureUnit::Texture0);
		opfor::Renderer::Submit(_quad.GetVertexArray());
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture0);

		opfor::Renderer::Shader::Pop();
	}
}

void MeshRenderer::RenderLight(glm::vec3 const &viewPos, float const exposure)
{
	// Lighting pass

	opfor::Renderer::Shader::Push(_light);

		UpdateLight();

		opfor::Renderer::Shader::SetUniform("viewPos", viewPos);
		opfor::Renderer::Shader::SetUniform("exposure", exposure);

		// Bind GBuffer Textures

		opfor::Renderer::PushTexture(_gBuffer.GetPositionTex(),   opfor::TextureUnit::Texture0);
		opfor::Renderer::PushTexture(_gBuffer.GetNormalTex(),     opfor::TextureUnit::Texture1);
		opfor::Renderer::PushTexture(_gBuffer.GetAlbedoSpecTex(), opfor::TextureUnit::Texture2);
		opfor::Renderer::PushTexture(_depthCubemap,               opfor::TextureUnit::Texture4);
		opfor::Renderer::PushTexture(_gBuffer.GetMetallicRoughnessTex(),         opfor::TextureUnit::Texture5);
		opfor::Renderer::PushTexture(TextureManager::Get().Get("irradianceMap"), opfor::TextureUnit::Texture6);
		opfor::Renderer::PushTexture(TextureManager::Get().Get("brdfLUT"),       opfor::TextureUnit::Texture7);
		opfor::Renderer::PushTexture(TextureManager::Get().Get("prefilterMap"),  opfor::TextureUnit::Texture8);

		opfor::Renderer::Submit(_quad.GetVertexArray());

		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture8);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture7);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture6);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture5);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture4);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture2);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture1);
		opfor::Renderer::PopTexture(opfor::TextureUnit::Texture0);

	opfor::Renderer::Shader::Pop();
}

void MeshRenderer::BakeShadowMap()
{
	//Logger::Info("Building shadow map\n");

	auto lights = opfor::Application::Get().GetEntities<PointLightComponent, TransformComponent>();
	auto players = opfor::Application::Get().GetEntities<PlayerCameraComponent, TransformComponent>();

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

	opfor::Renderer::PushViewport({ 0, 0 }, { ShadowWidth, ShadowHeight });
	opfor::Renderer::PushFramebuffer(_depthmap);

	opfor::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	opfor::Renderer::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);

	opfor::Renderer::PushCapability(opfor::RendererCaps::Blend, false);
	opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
	opfor::Renderer::Clear(opfor::ClearFlag::DepthBit);

	opfor::Renderer::Shader::Push(_shadow);
		opfor::Renderer::Shader::SetUniform("model", glm::mat4(1.0f));
		opfor::Renderer::Shader::SetUniform("shadowMatrices", shadowTransforms);
		opfor::Renderer::Shader::SetUniform("far_plane", 10000.0f);
		opfor::Renderer::Shader::SetUniform("lightPos", lightPos);

		RenderShadowMeshes();

	opfor::Renderer::Shader::Pop();

	opfor::Renderer::PopFramebuffer();
	opfor::Renderer::PopViewport();

	opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);
	opfor::Renderer::PopCapability(opfor::RendererCaps::Blend);
}

void MeshRenderer::RenderMeshes()
{
	auto player = opfor::Application::Get().GetEntities<PlayerCameraComponent, TransformComponent>();

	if (player.size() == 0) { return ; }

	auto [ playerCamera, playerTransform ] = player[0]->GetAll();

	BakeShadowMap();

	opfor::Renderer::PushFramebuffer(_gBuffer.GetFramebuffer());
		opfor::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		opfor::Renderer::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);

		opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
			RenderMeshes(playerCamera, playerTransform);
		opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);
	opfor::Renderer::PopFramebuffer();

	opfor::Renderer::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	opfor::Renderer::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);

	RenderLight(playerTransform.position, playerCamera.exposure);

	// Copy depth buffer to viewport framebuffer to enable depth testing with billboard
	// and other shaders
	auto const viewport = opfor::Application::Get().GetViewport();
	opfor::Renderer::CopyFramebufferToDefaultFramebuffer(_gBuffer.GetFramebuffer(), opfor::CopyTarget::DepthBufferBit);

	opfor::Renderer::PushCapability(opfor::RendererCaps::Blend, true);
	opfor::Renderer::PushCapability(opfor::RendererCaps::DepthTest, true);
		// RenderSSAO(playerCamera);
		RenderLightBillboard(playerCamera);
	opfor::Renderer::PopCapability(opfor::RendererCaps::DepthTest);
	opfor::Renderer::PopCapability(opfor::RendererCaps::Blend);
}

}