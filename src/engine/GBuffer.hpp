#pragma once

#include "lazy.hpp"
#include "Engine.hpp"
#include "engine/renderer/Texture.hpp"

class GBuffer
{
private:
	GLuint _gBuffer;
	GLuint _gDepth;
	opfor::UniquePtr<opfor::Texture> _gPosition;
	opfor::UniquePtr<opfor::Texture> _gNormal;
	opfor::UniquePtr<opfor::Texture> _gAlbedoSpec;
	opfor::UniquePtr<opfor::Texture> _gMetallicRoughness;

	void Init()
	{
		auto display = opfor::Engine::Get().GetWindow();
		auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

		glGenFramebuffers(1, &_gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

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

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition->GetRawHandle(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal->GetRawHandle(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec->GetRawHandle(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gMetallicRoughness->GetRawHandle(), 0);

		glGenRenderbuffers(1, &_gDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, _gDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _gDepth);

		GLuint st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		assert(st == GL_FRAMEBUFFER_COMPLETE);

		std::array<GLuint, 4> attachments = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		glDrawBuffers(attachments.size(), attachments.data());

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

	GLuint GetFramebufferId() const { return _gBuffer; }
	GLuint GetDepthTex() const { return _gDepth; }
	GLuint GetPositionTex() const { return _gPosition->GetRawHandle(); }
	GLuint GetNormalTex() const { return _gNormal->GetRawHandle(); }
	GLuint GetAlbedoSpecTex() const { return _gAlbedoSpec->GetRawHandle(); }
	GLuint GetMetallicRoughnessTex() const { return _gMetallicRoughness->GetRawHandle(); }
};
