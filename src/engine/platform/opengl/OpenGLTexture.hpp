#pragma once

#include "engine/renderer/Texture.hpp"

namespace opfor {

class OpenGLTexture : public Texture
{
private:
	uint32_t _RendererID;

	TextureType _Type;
	std::optional<TextureUnit> _BoundUnit;

public:
	OpenGLTexture(TextureType type, DataFormat input, DataFormat output,
		DataType dataType, size_t width, size_t height, void *data);

	virtual ~OpenGLTexture();

	void Bind(TextureUnit) override;
	void Unbind() override;

	void SetParameter(TextureParameter param) override;
	void SetParameters(TextureParameterList params) override;

	uint32_t GetRawHandle() const override { return _RendererID; }
};

}
