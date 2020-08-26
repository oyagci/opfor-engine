#pragma once

#include "renderer.hpp"
#include "opfor/renderer/Texture.hpp"
#include "opfor/renderer/Renderbuffer.hpp"
#include "opfor/renderer/Framebuffer.hpp"

class GBuffer
{
private:
	opfor::SharedPtr<opfor::Framebuffer> _gBuffer;
	opfor::SharedPtr<opfor::Texture> _gPosition;
	opfor::SharedPtr<opfor::Texture> _gNormal;
	opfor::SharedPtr<opfor::Texture> _gAlbedoSpec;
	opfor::SharedPtr<opfor::Texture> _gMetallicRoughness;
	opfor::SharedPtr<opfor::Texture> _gDepth;

	void Init();

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

	void Resize(unsigned int width, unsigned int height);

	auto const &GetFramebuffer() { return _gBuffer; }
	auto &GetPositionTex() const { return _gPosition; }
	auto &GetNormalTex() const { return _gNormal; }
	auto &GetAlbedoSpecTex() const { return _gAlbedoSpec; }
	auto &GetDepthTex() const { return _gDepth; }
	auto &GetMetallicRoughnessTex() const { return _gMetallicRoughness; }
};
