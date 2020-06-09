#pragma once

#include "lazy.hpp"
#include "Engine.hpp"

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
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
