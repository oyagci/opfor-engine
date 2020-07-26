#pragma once

#include "engine/renderer/Texture.hpp"

namespace opfor {

class OpenGLTexture2D : public Texture2D
{
	void ApplyParameters();

	uint32_t _RendererID = 0;
	void *_TextureData = nullptr;

public:
	OpenGLTexture2D();
	virtual ~OpenGLTexture2D();

	virtual void SetData(void *) override;

	void Bind(TextureUnit) override;

	uint32_t GetRawHandle() const override { return _RendererID; }

	void Build() override;
};

class OpenGLTextureCubemap : public TextureCubemap
{
	void ApplyParameters();

	uint32_t _RendererID = 0;
	void *_TextureData = nullptr;

public:
	OpenGLTextureCubemap();
	virtual ~OpenGLTextureCubemap();

	void Bind(TextureUnit) override;
	void SetFaceData(CubemapFace, ImageLoader::Image image) override;

	uint32_t GetRawHandle() const override { return _RendererID; }

	void Build() override;
};

}
