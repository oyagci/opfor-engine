#include "MeshRenderer.hpp"

#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include "opfor/core/Application.hpp"
#include "opfor/renderer/TextureManager.hpp"
#include "opfor/renderer/ShaderManager.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/core/ImageLoader.hpp"

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
	InitDepthCubemap();

	auto texture = TextureManager::Get().Create2D("light_bulb_icon");
	auto img = ImageLoader::Load("./img/light_bulb_icon.png");

	TextureParameterList texParams = {
		{ TextureParameterType::MignifyFilter, TextureParameterValue::LinearMipmapLinear },
		{ TextureParameterType::MagnifyFilter, TextureParameterValue::Linear },
		{ TextureParameterType::WrapT,         TextureParameterValue::ClampToEdge },
		{ TextureParameterType::WrapS,         TextureParameterValue::ClampToEdge },
	};

	texture->SetDataType(DataType::UnsignedByte);
	texture->SetHasAlpha(img.nchannel == 4);
	texture->SetIsSRGB(true);
	texture->SetInputFormat(img.nchannel == 4 ? DataFormat::RGBA : DataFormat::RGB);
	texture->SetOutputFormat(img.nchannel == 4 ? DataFormat::RGBA : DataFormat::RGB);
	texture->SetSize(img.width, img.height);
	texture->SetTextureParameters(texParams);
	texture->SetGenerateMipmap(true);
	texture->SetData(img.data.get());
	texture->Build();

	_light = Shader::Create("shaders/light.glsl");

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
	_shadow = Shader::Create("shaders/shadow.glsl");

	float aspect = static_cast<float>(ShadowWidth) / static_cast<float>(ShadowHeight);
	float n = 1.0f;
	float f = 1000.0f;
	_shadowProjection = glm::perspective(glm::radians(90.0f), aspect, n, f);

	_depthCubemap = TextureCubemap::Create();

	_depthCubemap->SetTextureParameters({
		{ TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest },
		{ TextureParameterType::MignifyFilter, TextureParameterValue::Nearest },
		{ TextureParameterType::WrapR,         TextureParameterValue::ClampToEdge },
		{ TextureParameterType::WrapS,         TextureParameterValue::ClampToEdge },
		{ TextureParameterType::WrapT,         TextureParameterValue::ClampToEdge },
	});
	_depthCubemap->SetInputFormat(DataFormat::Depth);
	_depthCubemap->SetOutputFormat(DataFormat::Depth);
	_depthCubemap->SetDataType(DataType::Float);
	_depthCubemap->SetSize(ShadowWidth, ShadowHeight);

	_depthCubemap->Bind(TextureUnit::Texture0);

	constexpr std::array<CubemapFace, 6> faces {
		CubemapFace::PositiveX, CubemapFace::NegativeX,
		CubemapFace::PositiveY, CubemapFace::NegativeY,
		CubemapFace::PositiveZ, CubemapFace::NegativeZ,
	};

	_depthCubemap->Bind(TextureUnit::Texture0);

	for (size_t i = 0; i < faces.size(); i++) {
		ImageLoader::Image img{};

		img.width = ShadowWidth;
		img.height = ShadowHeight;

		_depthCubemap->SetFaceData(faces[i], std::move(img));
	}

	_depthCubemap->Build();

	_depthmap = Framebuffer::Create();
	_depthmap->Bind();
	_depthmap->AttachTexture(_depthCubemap, FramebufferAttachment::DepthAttachment);
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
	_billboard = Shader::Create("shaders/billboard.glsl");
}

void MeshRenderer::RenderShadowMeshes()
{
	auto entities = Application::Get().GetEntities<ModelComponent, TransformComponent>();

	for (auto const entity: entities) {

		auto [ model, transform ] = entity->GetAll();

		for (auto const meshId : model.Meshes) {

			auto const *mesh = Application::Get().GetMesh(meshId);

			glm::mat4 model(1.0f);
			model = glm::translate(model, transform.position);
			model = glm::scale(model, transform.scale);
			Renderer::Shader::SetUniform("modelMatrix", model);

			Renderer::Submit(reinterpret_cast<Mesh const*>(mesh)->GetVertexArray());
		}

	}
}

void MeshRenderer::RenderMeshes(PerspectiveCamera const &camera)
{
	auto models = Application::Get().GetEntities<ModelComponent, TransformComponent>();

	for (auto const &entity : models) {

		auto const [ model, transform ] = entity->GetAll();

		for (auto const meshId : model.Meshes) {

			auto const mesh = Application::Get().GetMesh(meshId);
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

			Renderer::Shader::Push(shader);
			Renderer::Shader::SetUniform("viewProjectionMatrix", camera.GetViewProjectionMatrix());
			Renderer::Shader::SetUniform("viewMatrix", camera.GetViewMatrix());
			Renderer::Shader::SetUniform("projectionMatrix", camera.GetProjection());
			Renderer::Shader::SetUniform("viewPos", camera.GetPosition());
			Renderer::Shader::SetUniform("modelMatrix", modelMatrix);

			bool popAlbedo = false;
			bool popMetallic = false;
			bool popNormal = false;

			// Bind each texture
			auto meshCast = dynamic_cast<Mesh*>(mesh);
			if (meshCast != nullptr) {
				if (meshCast->GetPbrMaterial().has_value()) {

					auto material = Application::Get().GetPbrMaterial(
						meshCast->GetPbrMaterial().value());

					if (material.has_value()) {
						auto m = material.value();

						Renderer::Shader::SetUniform("material.baseColor", m->BaseColor);
						Renderer::Shader::SetUniform("material.metallicFactor", m->MetallicFactor);
						Renderer::Shader::SetUniform("material.roughnessFactor", m->RoughnessFactor);

						if (m->Albedo.has_value()) {
							auto albedo = m->Albedo.value();
							auto texture = TextureManager::Get().Get(albedo);

							popAlbedo = true;

							Renderer::Shader::SetUniform("material.hasAlbedo", 1);
							Renderer::PushTexture(texture, TextureUnit::Texture0);
						}
						else {
							Renderer::Shader::SetUniform("material.hasAlbedo", 0);
						}

						if (m->MetallicRoughness.has_value()) {
							auto metallicRoughness = m->MetallicRoughness.value();
							auto texture = TextureManager::Get().Get(metallicRoughness);

							popMetallic = true;

							Renderer::Shader::SetUniform("material.hasMetallicRoughness", 1);
							Renderer::PushTexture(texture, TextureUnit::Texture1);
						}
						else {
							Renderer::Shader::SetUniform("material.hasMetallicRoughness", 0);
						}

						if (m->Normal.has_value()) {
							auto normal = m->Normal.value();
							auto texture = TextureManager::Get().Get(normal);

							popNormal = true;

							Renderer::PushTexture(texture, TextureUnit::Texture2);
						}
						else {
							auto const defaultNormal = TextureManager::Get().Get("default_normal");
							Renderer::PushTexture(defaultNormal, TextureUnit::Texture2);

							popNormal = true;
						}
					}
				}
			}

			Renderer::Submit(reinterpret_cast<Mesh const*>(mesh)->GetVertexArray());

			popAlbedo   ? Renderer::PopTexture(TextureUnit::Texture0) : (void)0;
			popMetallic ? Renderer::PopTexture(TextureUnit::Texture1) : (void)0;
			popNormal   ? Renderer::PopTexture(TextureUnit::Texture2) : (void)0;

			Renderer::Shader::Pop();
		}

	}
}

void MeshRenderer::UpdateLight()
{
	auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

	if (lights.size() == 0) { return ; }

	Renderer::Shader::SetUniform("pointLightCount", lights.size());

	for (size_t i = 0; i < lights.size(); i++) {
		auto [ light, transform ] = lights[i]->GetAll();

		std::string pointLight = "pointLight[" + std::to_string(i) + "]";

		Renderer::Shader::SetUniform(pointLight + ".position", transform.position);
		Renderer::Shader::SetUniform(pointLight + ".color", light.Color);
		Renderer::Shader::SetUniform(pointLight + ".intensity", light.Intensity);
	}

	auto dirLights = Application::Get().GetEntities<DirectionalLightComponent>();

	Renderer::Shader::SetUniform("directionalLightCount", dirLights.size());

	for (size_t i = 0; i < dirLights.size(); i++) {
		auto [ light ] = dirLights[i]->GetAll();

		std::string directionalLight = "directionalLights[" + std::to_string(i) + "]";

		Renderer::Shader::SetUniform(directionalLight + ".direction", light.Direction);
		Renderer::Shader::SetUniform(directionalLight + ".color", light.Color);
		Renderer::Shader::SetUniform(directionalLight + ".intensity", light.Intensity);
	}
}

void MeshRenderer::RenderLightBillboard(PerspectiveCamera const &camera)
{
	auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

	if (lights.size() == 0 ) { return ; }

	for (auto const &lightEnt : lights) {
		auto [ light, transform ] = lightEnt->GetAll();

		Renderer::Shader::Push(_billboard);
		Renderer::Shader::SetUniform("viewMatrix", camera.GetViewMatrix());
		Renderer::Shader::SetUniform("viewProjectionMatrix", camera.GetViewProjectionMatrix());
		Renderer::Shader::SetUniform("projectionMatrix", camera.GetProjection());
		Renderer::Shader::SetUniform("particlePosition", transform.position);

		Renderer::PushTexture(TextureManager::Get().Get("light_bulb_icon"), TextureUnit::Texture0);
		Renderer::Submit(_quad.GetVertexArray());
		Renderer::PopTexture(TextureUnit::Texture0);

		Renderer::Shader::Pop();
	}
}

void MeshRenderer::RenderLight(PerspectiveCamera const &camera)
{
	Renderer::Shader::Push(_light);

		UpdateLight();

		Renderer::Shader::SetUniform("viewPos", camera.GetPosition());
		Renderer::Shader::SetUniform("exposure", camera.GetExposure());

		Renderer::PushTexture(_gBuffer.GetPositionTex(),   TextureUnit::Texture0);
		Renderer::PushTexture(_gBuffer.GetNormalTex(),     TextureUnit::Texture1);
		Renderer::PushTexture(_gBuffer.GetAlbedoSpecTex(), TextureUnit::Texture2);
		Renderer::PushTexture(_depthCubemap,               TextureUnit::Texture4);
		Renderer::PushTexture(_gBuffer.GetMetallicRoughnessTex(),         TextureUnit::Texture5);
		Renderer::PushTexture(TextureManager::Get().Get("irradianceMap"), TextureUnit::Texture6);
		Renderer::PushTexture(TextureManager::Get().Get("brdfLUT"),       TextureUnit::Texture7);
		Renderer::PushTexture(TextureManager::Get().Get("prefilterMap"),  TextureUnit::Texture8);

		Renderer::Submit(_quad.GetVertexArray());

		Renderer::PopTexture(TextureUnit::Texture8);
		Renderer::PopTexture(TextureUnit::Texture7);
		Renderer::PopTexture(TextureUnit::Texture6);
		Renderer::PopTexture(TextureUnit::Texture5);
		Renderer::PopTexture(TextureUnit::Texture4);
		Renderer::PopTexture(TextureUnit::Texture2);
		Renderer::PopTexture(TextureUnit::Texture1);
		Renderer::PopTexture(TextureUnit::Texture0);

	Renderer::Shader::Pop();
}

void MeshRenderer::BakeShadowMap()
{
	auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

	if (lights.size() == 0) return ;

	auto lightPos = lights[0]->Get<TransformComponent>().position;

	std::vector<glm::mat4> shadowTransforms = {
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
		_shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)),
	};

	Renderer::PushViewport({ 0, 0 }, { ShadowWidth, ShadowHeight });
	Renderer::PushFramebuffer(_depthmap);

	Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	Renderer::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);

	Renderer::PushCapability(RendererCaps::Blend, false);
	Renderer::PushCapability(RendererCaps::DepthTest, true);
	Renderer::Clear(ClearFlag::DepthBit);

	Renderer::Shader::Push(_shadow);
		Renderer::Shader::SetUniform("model", glm::mat4(1.0f));
		Renderer::Shader::SetUniform("shadowMatrices", shadowTransforms);
		Renderer::Shader::SetUniform("far_plane", 10000.0f);
		Renderer::Shader::SetUniform("lightPos", lightPos);

		RenderShadowMeshes();

	Renderer::Shader::Pop();

	Renderer::PopFramebuffer();
	Renderer::PopViewport();

	Renderer::PopCapability(RendererCaps::DepthTest);
	Renderer::PopCapability(RendererCaps::Blend);
}

void MeshRenderer::RenderMeshes()
{
	BakeShadowMap();

	auto &camera = Application::Get().GetCameraController().GetCamera();

	Renderer::PushFramebuffer(_gBuffer.GetFramebuffer());
		Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Renderer::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);

		Renderer::PushCapability(RendererCaps::DepthTest, true);
			RenderMeshes(camera);
		Renderer::PopCapability(RendererCaps::DepthTest);
	Renderer::PopFramebuffer();

	Renderer::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	Renderer::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);

	RenderLight(camera);

	// Copy depth buffer to viewport framebuffer to enable depth testing with billboard
	// and other shaders
	Renderer::CopyFramebufferToDefaultFramebuffer(_gBuffer.GetFramebuffer(), CopyTarget::DepthBufferBit);

	Renderer::PushCapability(RendererCaps::Blend, true);
	Renderer::PushCapability(RendererCaps::DepthTest, true);
		RenderLightBillboard(camera);
	Renderer::PopCapability(RendererCaps::DepthTest);
	Renderer::PopCapability(RendererCaps::Blend);
}

void MeshRenderer::Resize(unsigned int width, unsigned int height)
{
	_gBuffer.Resize(width, height);
}

}
