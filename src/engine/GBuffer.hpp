#pragma once

#include "lazy.hpp"
#include "Engine.hpp"
#include "engine/renderer/Texture.hpp"
#include "engine/renderer/Renderbuffer.hpp"
#include "engine/renderer/Framebuffer.hpp"

class GBuffer
{
private:
	opfor::SharedPtr<opfor::Framebuffer> _gBuffer;
	opfor::SharedPtr<opfor::Texture> _gPosition;
	opfor::SharedPtr<opfor::Texture> _gNormal;
	opfor::SharedPtr<opfor::Texture> _gAlbedoSpec;
	opfor::SharedPtr<opfor::Texture> _gMetallicRoughness;
	opfor::SharedPtr<opfor::Renderbuffer> _gDepth;

	void Init()
	{
		auto display = opfor::Engine::Get().GetWindow();
		auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

		_gBuffer = opfor::Framebuffer::Create();
		_gBuffer->Bind();

		using namespace opfor;

		TextureParameterList const params = {
			{ TextureParameterType::MignifyFilter, TextureParameterValue::Nearest },
			{ TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest },
			{ TextureParameterType::WrapR,         TextureParameterValue::ClampToEdge },
			{ TextureParameterType::WrapS,         TextureParameterValue::ClampToEdge },
			{ TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest },
		};

		_gPosition = opfor::Texture::Create(TextureType::Tex2D, params, DataFormat::RGBA16F,
				DataFormat::RGBA, DataType::Float, width, height, 0, nullptr);

		_gNormal = opfor::Texture::Create(TextureType::Tex2D, params, DataFormat::RGBA16F,
				DataFormat::RGBA, DataType::Float, width, height, 0, nullptr);

		_gAlbedoSpec = opfor::Texture::Create(TextureType::Tex2D, params, DataFormat::RGBA16F,
				DataFormat::RGBA, DataType::Float, width, height, 0, nullptr);

		_gMetallicRoughness = opfor::Texture::Create(TextureType::Tex2D, params, DataFormat::RGBA16F,
				DataFormat::RGBA, DataType::Float, width, height, 0, nullptr);

		_gDepth = Renderbuffer::Create();
		_gDepth->AttachDepthComponent({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

		_gBuffer->AttachTexture(_gPosition,          FramebufferAttachment::ColorAttachment0);
		_gBuffer->AttachTexture(_gNormal,            FramebufferAttachment::ColorAttachment1);
		_gBuffer->AttachTexture(_gAlbedoSpec,        FramebufferAttachment::ColorAttachment2);
		_gBuffer->AttachTexture(_gMetallicRoughness, FramebufferAttachment::ColorAttachment3);

		_gBuffer->AttachRenderbuffer(_gDepth, RenderbufferAttachment::DepthAttachment);

		OP4_CORE_ASSERT(_gBuffer->IsComplete(), "Incomplete Framebuffer!\n");

		_gBuffer->Unbind();
	}

public:
	GBuffer()
	{
		Init();
	}

	~GBuffer() = default;

	void Bind()
	{
		_gBuffer->Bind();
	}

	void Unbind()
	{
		_gBuffer->Unbind();
	}

	auto const &GetFramebuffer() { return _gBuffer; }
	auto &GetPositionTex() const { return _gPosition; }
	auto &GetNormalTex() const { return _gNormal; }
	auto &GetAlbedoSpecTex() const { return _gAlbedoSpec; }
	GLuint GetDepthTex() const { return _gDepth->GetRawHandle(); }
	auto &GetMetallicRoughnessTex() const { return _gMetallicRoughness; }
};
