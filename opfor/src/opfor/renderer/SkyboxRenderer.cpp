#include "SkyboxRenderer.hpp"
#include "opfor/core/Application.hpp"
#include "opfor/renderer/TextureManager.hpp"

#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "components/SkyboxComponent.hpp"

namespace opfor {

SkyboxRenderer::SkyboxRenderer()
{
	_shader = opfor::Shader::Create("shaders/skybox.glsl");
	_SphericalToCubemap = opfor::Shader::Create("shaders/hdri-cubemap-projection.glsl");

	InitQuad();
	InitSkybox();
	InitHDRI();
}

void SkyboxRenderer::InitQuad()
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
		_Quad.addPosition(p);
	}
	for (auto t : tex) {
		_Quad.addUv(t);
	}
	_Quad.addTriangle(glm::uvec3(0, 1, 2));
	_Quad.addTriangle(glm::uvec3(0, 2, 3));
	_Quad.build();
}

opfor::SharedPtr<opfor::Texture2D> SkyboxRenderer::LoadHDRI()
{
	stbi_set_flip_vertically_on_load(1);
	auto image = opfor::ImageLoader::LoadHDR("assets/hdri/ballroom_4k.hdr");
	stbi_set_flip_vertically_on_load(0);

	auto texture = opfor::Texture2D::Create();
	texture->SetData(image.data.get());
	texture->SetDataType(opfor::DataType::Float);
	texture->SetInputFormat(opfor::DataFormat::RGB16F);
	texture->SetOutputFormat(opfor::DataFormat::RGB);
	texture->SetSize(image.width, image.height);
	texture->SetIsSRGB(true);
	texture->SetTextureParameters({
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapS, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT, opfor::TextureParameterValue::ClampToEdge },
	});

	texture->Build();

	return texture;
}

void SkyboxRenderer::InitHDRI()
{
	opfor::SharedPtr<opfor::Texture2D> hdrTexture = LoadHDRI();
	opfor::SharedPtr<opfor::Framebuffer> capture = opfor::Framebuffer::Create();
	opfor::SharedPtr<opfor::Renderbuffer> captureRbo = opfor::Renderbuffer::Create();

	captureRbo->AttachDepthComponent({ 2048, 2048 });
	capture->AttachRenderbuffer(captureRbo, opfor::RenderbufferAttachment::DepthAttachment);

	opfor::SharedPtr<opfor::TextureCubemap> envCubemap = opfor::TextureCubemap::Create();
	envCubemap->SetInputFormat(opfor::DataFormat::RGB16F);
	envCubemap->SetOutputFormat(opfor::DataFormat::RGB);
	envCubemap->SetSize(2048, 2048);
	envCubemap->SetDataType(opfor::DataType::Float);
	envCubemap->SetTextureParameters({
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapS, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapR, opfor::TextureParameterValue::ClampToEdge },
	});

	opfor::ImageLoader::Image emptyImage{};
		emptyImage.width = 2048;
		emptyImage.height = 2048;

	envCubemap->Bind(opfor::TextureUnit::Texture0);
	for (size_t i = 0; i < 6; i++) {
		envCubemap->SetFaceData((opfor::CubemapFace)((GLint)opfor::CubemapFace::PositiveX + i), std::move(emptyImage));
	}

	envCubemap->Build();

	glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));
	std::array<glm::mat4, 6> views = {
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	_SphericalToCubemap->Bind();
	_SphericalToCubemap->SetUniform("equirectangularMap", 0);
	_SphericalToCubemap->SetUniform("projectionMatrix", projection);
	hdrTexture->Bind(opfor::TextureUnit::Texture0);
	opfor::RenderCommand::PushViewport({ 0, 0 }, { 2048, 2048 });

	capture->Bind();

	for (size_t i = 0; i < 6; i++) {
		_SphericalToCubemap->SetUniform("viewMatrix", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->GetRawHandle(), 0);
		opfor::RenderCommand::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);
		_SkyboxCubeMesh.Draw();
	}

	capture->Unbind();

	opfor::RenderCommand::PopViewport();
	_SphericalToCubemap->Unbind();

	_HDRI = envCubemap;

	////////////////////////////////////////////////////////

	auto irradianceShader = opfor::Shader::Create("shaders/convolute.glsl");
	_Irradiance = opfor::TextureCubemap::Create();
	_Irradiance->SetInputFormat(opfor::DataFormat::RGB16F);
	_Irradiance->SetOutputFormat(opfor::DataFormat::RGB);
	_Irradiance->SetDataType(opfor::DataType::Float);
	_Irradiance->SetSize(32, 32);
	_Irradiance->SetTextureParameters({
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapS, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapR, opfor::TextureParameterValue::ClampToEdge },
	});

	emptyImage.width = 32;
	emptyImage.height = 32;

	_Irradiance->Bind(opfor::TextureUnit::Texture0);
	for (size_t i = 0; i < 6; i++) {
		_Irradiance->SetFaceData((opfor::CubemapFace)((GLint)opfor::CubemapFace::PositiveX + i), std::move(emptyImage));
	}
	_Irradiance->Build();

	capture->Bind();
	captureRbo->Bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradianceShader->Bind();
	irradianceShader->SetUniform("cubemap", 0);
	irradianceShader->SetUniform("projectionMatrix", projection);
	envCubemap->Bind(opfor::TextureUnit::Texture0);
	opfor::RenderCommand::PushViewport({ 0, 0 }, { 32, 32 });

	for (size_t i = 0; i < 6; i++) {
		irradianceShader->SetUniform("viewMatrix", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _Irradiance->GetRawHandle(), 0);
		opfor::RenderCommand::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);
		_SkyboxCubeMesh.Draw();
	}
	capture->Unbind();
	opfor::RenderCommand::PopViewport();
	irradianceShader->Unbind();

	TextureManager::Get().Add("irradianceMap", _Irradiance);

	///////////////////////////////////////////////////////

	opfor::SharedPtr<opfor::Shader> prefilterShader = opfor::Shader::Create("shaders/prefilter.glsl");

	emptyImage.width = 128;
	emptyImage.height = 128;

	_Prefilter = opfor::TextureCubemap::Create();
	_Prefilter->SetInputFormat(opfor::DataFormat::RGB16F);
	_Prefilter->SetOutputFormat(opfor::DataFormat::RGB);
	_Prefilter->SetDataType(opfor::DataType::Float);
	_Prefilter->SetSize(128, 128);
	_Prefilter->SetTextureParameters({
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::LinearMipmapLinear },
		{ opfor::TextureParameterType::WrapS, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT, opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapR, opfor::TextureParameterValue::ClampToEdge },
	});
	_Prefilter->SetGenerateMipmap(true);
	_Prefilter->Bind(opfor::TextureUnit::Texture0);
	for (size_t i = 0; i < 6; i++) {
		envCubemap->SetFaceData((opfor::CubemapFace)((GLint)opfor::CubemapFace::PositiveX + i), std::move(emptyImage));
	}
	_Prefilter->Build();

	opfor::RenderCommand::PushShader(prefilterShader);

	opfor::RenderCommand::SetUniform("cubemap", 0);
	opfor::RenderCommand::SetUniform("projectionMatrix", projection);

	opfor::RenderCommand::PushTexture(_HDRI, opfor::TextureUnit::Texture0);
	opfor::RenderCommand::PushFramebuffer(capture);

	size_t maxMipLevel = 5;
	for (size_t mip = 0; mip < maxMipLevel; mip++) {

		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);

		captureRbo->Bind();
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		opfor::RenderCommand::PushViewport({ 0, 0 }, { mipWidth, mipHeight });

		float roughness = (float)mip / (float)(maxMipLevel - 1);
		prefilterShader->SetUniform("roughness", roughness);

		for (unsigned int i = 0; i < 6; i++) {
			prefilterShader->SetUniform("viewMatrix", views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				_Prefilter->GetRawHandle(), mip);
			opfor::RenderCommand::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);
			_SkyboxCubeMesh.Draw();
		}
		opfor::RenderCommand::PopViewport();
	}

	opfor::RenderCommand::PopFramebuffer();
	opfor::RenderCommand::PopTexture(opfor::TextureUnit::Texture0);
	opfor::RenderCommand::PopShader();

	TextureManager::Get().Add("prefilterMap", _Prefilter);

	////////////////////////////////////////////////////////////////////////

	opfor::SharedPtr<opfor::Shader> brdfShader = opfor::Shader::Create("shaders/brdf-precompute.glsl");

	_BRDF = opfor::Texture2D::Create();
	_BRDF->SetTextureParameters({
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Linear },
		{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::ClampToEdge },
	});
	_BRDF->SetInputFormat(opfor::DataFormat::RGB16F);
	_BRDF->SetOutputFormat(opfor::DataFormat::RG);
	_BRDF->SetDataType(opfor::DataType::Float);
	_BRDF->SetSize(512, 512);
	_BRDF->Build();

	opfor::RenderCommand::PushFramebuffer(capture);

	captureRbo->Bind();
	// capture->AttachTexture(_BRDF, opfor::FramebufferAttachment::ColorAttachment0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _BRDF->GetRawHandle(), 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	opfor::RenderCommand::PushViewport({ 0, 0 }, { 512, 512 });
	opfor::RenderCommand::PushShader(brdfShader);

	opfor::RenderCommand::Clear(opfor::ClearFlag::ColorBit | opfor::ClearFlag::DepthBit);

	_Quad.Draw();

	opfor::RenderCommand::PopShader();
	opfor::RenderCommand::PopViewport();

	TextureManager::Get().Add("brdfLUT", _BRDF);
}

void SkyboxRenderer::InitSkybox()
{
	const std::array<std::string, 6> paths = {
		"./img/right.jpg",
		"./img/left.jpg",
		"./img/top.jpg",
		"./img/bottom.jpg",
		"./img/front.jpg",
		"./img/back.jpg",
	};

	opfor::TextureParameterList params = {
		{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Nearest },
		{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Nearest },
		{ opfor::TextureParameterType::WrapR,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapS,         opfor::TextureParameterValue::ClampToEdge },
		{ opfor::TextureParameterType::WrapT,         opfor::TextureParameterValue::ClampToEdge },
	};

	_SkyboxCubemap = opfor::TextureCubemap::Create();
	_SkyboxCubemap->SetTextureParameters(params);
	_SkyboxCubemap->SetInputFormat(opfor::DataFormat::RGB);
	_SkyboxCubemap->SetOutputFormat(opfor::DataFormat::RGB);
	_SkyboxCubemap->SetDataType(opfor::DataType::UnsignedByte);

	stbi_set_flip_vertically_on_load(false);

	constexpr std::array<opfor::CubemapFace, 6> faces {
		opfor::CubemapFace::PositiveX, opfor::CubemapFace::NegativeX,
		opfor::CubemapFace::PositiveY, opfor::CubemapFace::NegativeY,
		opfor::CubemapFace::PositiveZ, opfor::CubemapFace::NegativeZ,
	};

	_SkyboxCubemap->Bind(opfor::TextureUnit::Texture0);

	for (size_t i = 0; i < faces.size(); i++) {
		opfor::ImageLoader::Image img = opfor::ImageLoader::Load(paths[i]);
		_SkyboxCubemap->SetFaceData(faces[i], std::move(img));
	}

	_SkyboxCubemap->Build();

	TextureManager::Get().Add("skybox-cubemap", _SkyboxCubemap);

	const glm::vec3 verts[] = {
		glm::vec3(-1.0, -1.0,  1.0),
		glm::vec3( 1.0, -1.0,  1.0),
		glm::vec3( 1.0,  1.0,  1.0),
		glm::vec3(-1.0,  1.0,  1.0),

		glm::vec3(-1.0, -1.0, -1.0),
		glm::vec3( 1.0, -1.0, -1.0),
		glm::vec3( 1.0,  1.0, -1.0),
		glm::vec3(-1.0,  1.0, -1.0),
	};

	const glm::uvec3 indices[] = {
		// front
		glm::uvec3(0, 3, 1),
		glm::uvec3(1, 3, 2),
		// right
		glm::uvec3(1, 2, 5),
		glm::uvec3(2, 6, 5),
		// back
		glm::uvec3(4, 5, 6),
		glm::uvec3(4, 6, 7),
		// left
		glm::uvec3(0, 4, 7),
		glm::uvec3(0, 7, 3),
		// bottom
		glm::uvec3(0, 1, 4),
		glm::uvec3(1, 5, 4),
		// top
		glm::uvec3(3, 7, 2),
		glm::uvec3(2, 7, 6)
	};

	_SkyboxCubeMesh.addTexture("skybox-cubemap", opfor::Mesh::TextureType::TT_Diffuse);
	for (const auto &v : verts) { _SkyboxCubeMesh.addPosition(v); }
	for (const auto &i : indices) { _SkyboxCubeMesh.addTriangle(i); }
	_SkyboxCubeMesh.build();
}

void SkyboxRenderer::RenderSkybox(PerspectiveCamera const &camera)
{
	auto skybox = opfor::Application::Get().GetEntities<SkyboxComponent>();

	if (skybox.size() == 0) { return ; }

	RenderCommandBuffer renderCommand;
	
	DrawCommand drawCommand;
		drawCommand.shader = _shader;
		drawCommand.uniformBindings = {
			{ "viewMatrix", glm::mat4(glm::mat3(camera.GetViewMatrix())) },
			{ "projectionMatrix", camera.GetProjection() },
		};
		drawCommand.textureBindings = {
			{ _HDRI, TextureUnit::Texture0 }
		};
		drawCommand.vertexArray = _SkyboxCubeMesh.GetVertexArray();

	renderCommand.drawCommands = { drawCommand };

	opfor::Renderer::SetDepthMask(false);
	Renderer::SubmitRenderCommandBuffer(renderCommand);
	opfor::Renderer::SetDepthMask(true);

}

}
