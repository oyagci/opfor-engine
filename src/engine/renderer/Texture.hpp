#pragma once

#include "lazy.hpp"
#include "engine/core/base.hpp"

namespace opfor {

enum class TextureType
{
#ifdef OP4_PLATFORM_OPENGL
	Tex1D = GL_TEXTURE_1D,
	Tex2D = GL_TEXTURE_2D,
	Tex3D = GL_TEXTURE_3D,
#endif
};

enum class DataFormat
{
#ifdef OP4_PLATFORM_OPENGL
	RGB     = GL_RGB,
	RGBA    = GL_RGBA,
	RGBA16F = GL_RGBA16F,
	RGB16F  = GL_RGB16F,
#endif
};

enum class DataType
{
#ifdef OP4_PLATFORM_OPENGL
	Int          = GL_INT,
	Float        = GL_FLOAT,
	UnsignedByte = GL_UNSIGNED_BYTE,
	UnsignedInt  = GL_UNSIGNED_INT,
#endif
};

enum class TextureParameterType
{
#ifdef OP4_PLATFORM_OPENGL
	MignifyFilter = GL_TEXTURE_MIN_FILTER,
	MagnifyFilter = GL_TEXTURE_MAG_FILTER,
	WrapR         = GL_TEXTURE_WRAP_R,
	WrapS         = GL_TEXTURE_WRAP_S,
	WrapT         = GL_TEXTURE_WRAP_T,
#endif
};

enum class TextureParameterValue
{
#ifdef OP4_PLATFORM_OPENGL
	Nearest              = GL_NEAREST,
	Linear               = GL_LINEAR,
	NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
	NearestMipmapLinear  = GL_NEAREST_MIPMAP_LINEAR,
	LinearMipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
	LineraMipmapLinear   = GL_LINEAR_MIPMAP_LINEAR,
	ClampToEdge          = GL_CLAMP_TO_EDGE,
	ClampToBorder        = GL_CLAMP_TO_BORDER,
#endif
};

enum class TextureUnit
{
#ifdef OP4_PLATFORM_OPENGL
	Texture0  = GL_TEXTURE0,
	Texture1  = GL_TEXTURE1,
	Texture2  = GL_TEXTURE2,
	Texture3  = GL_TEXTURE3,
	Texture4  = GL_TEXTURE4,
	Texture5  = GL_TEXTURE5,
	Texture6  = GL_TEXTURE6,
	Texture7  = GL_TEXTURE7,
	Texture8  = GL_TEXTURE8,
	Texture9  = GL_TEXTURE9,
	Texture10 = GL_TEXTURE10,
	Texture11 = GL_TEXTURE11,
	Texture12 = GL_TEXTURE12,
	Texture13 = GL_TEXTURE13,
	Texture14 = GL_TEXTURE14,
	Texture15 = GL_TEXTURE15,
#endif
};

using TextureParameter = std::pair<TextureParameterType, TextureParameterValue>;
using TextureParameterList = std::vector<TextureParameter>;

class Texture
{
public:
	virtual ~Texture() {}

	virtual void Bind(TextureUnit) = 0;
	virtual void Unbind() = 0;

	virtual void SetParameter(TextureParameter) = 0;
	virtual void SetParameters(TextureParameterList) = 0;

	virtual uint32_t GetRawHandle() const = 0;

	static UniquePtr<Texture> Create(TextureType type, TextureParameterList,
		DataFormat inputFormat, DataFormat outputFormat, DataType, size_t width, size_t height,
		size_t depth, void *data);
};

}
