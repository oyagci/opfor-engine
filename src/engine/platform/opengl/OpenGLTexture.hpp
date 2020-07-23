#pragma once

#include "engine/renderer/Texture.hpp"

namespace opfor {

class OpenGLTexture : public Texture
{
private:
	uint32_t _RendererID;

	std::optional<TextureUnit> _BoundUnit;

public:
	OpenGLTexture();

	virtual ~OpenGLTexture();

	void Bind(TextureUnit) override;
	void Unbind() override;

	uint32_t GetRawHandle() const override { return _RendererID; }

	void Build() override;
};

}
