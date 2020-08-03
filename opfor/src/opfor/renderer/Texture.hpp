#pragma once

#include "renderer.hpp"
#include "opfor/core/base.hpp"
#include "opfor/core/ImageLoader.hpp"
#include <glm/glm.hpp>

namespace opfor {

enum class CubemapFace
{
	PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

enum class TextureType
{
#ifdef OP4_PLATFORM_OPENGL
	Tex1D = GL_TEXTURE_1D,
	Tex2D = GL_TEXTURE_2D,
	Tex3D = GL_TEXTURE_3D,
	TexCubemap = GL_TEXTURE_CUBE_MAP,
#endif
};

enum class DataFormat
{
#ifdef OP4_PLATFORM_OPENGL
	RG      = GL_RG,
	RGB     = GL_RGB,
	RGBA    = GL_RGBA,
	RGBA16F = GL_RGBA16F,
	RGB16F  = GL_RGB16F,
	Depth   = GL_DEPTH_COMPONENT,
#endif
};

enum class DataType
{
#ifdef OP4_PLATFORM_OPENGL
	Int            = GL_INT,
	Float          = GL_FLOAT,
	UnsignedByte   = GL_UNSIGNED_BYTE,
	UnsignedInt    = GL_UNSIGNED_INT,
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
	LinearMipmapLinear   = GL_LINEAR_MIPMAP_LINEAR,
	ClampToEdge          = GL_CLAMP_TO_EDGE,
	ClampToBorder        = GL_CLAMP_TO_BORDER,
	Repeat               = GL_REPEAT,
	MirroredRepeat       = GL_MIRRORED_REPEAT,
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
private:
	DataFormat _InputFormat = DataFormat::RGB;
	DataFormat _OutputFormat = DataFormat::RGB;
	DataType _DataType = DataType::Int;
	TextureParameterList _Parameters;
	glm::vec3 _Size;

	bool _HasAlpha = false;
	bool _IsSRGB = false;
	bool _GenMipmap = false;

	TextureType _Type = TextureType::Tex2D;

protected:
	void SetTextureType(TextureType type) { _Type = type; }

public:
	virtual ~Texture() {}

	virtual void Bind(TextureUnit) = 0;

	virtual uint32_t GetRawHandle() const = 0;

	virtual void Build() = 0;

	void SetInputFormat(DataFormat input) { _InputFormat = input; }
	void SetOutputFormat(DataFormat output) { _OutputFormat = output; }
	void SetDataType(DataType dataType) { _DataType = dataType; }
	void SetHasAlpha(bool alpha) { _HasAlpha = alpha; }
	void SetIsSRGB(bool srgb) { _IsSRGB = srgb; }
	void SetTextureParameters(TextureParameterList paramList) { _Parameters = paramList; }
	void SetSize(float width, float height = 0, float depth = 0) {
		_Size.x = width;
		_Size.y = height;
		_Size.z = depth;
	}
	void SetGenerateMipmap(bool val) { _GenMipmap = val; }

	auto GetInputFormat() const { return _InputFormat; }
	auto GetOutputFormat() const { return _OutputFormat; }
	auto GetDataType() const { return _DataType; }
	auto HasAlpha() const { return _HasAlpha; }
	auto IsSRGB() const { return _IsSRGB; }
	auto const &GetTextureParameters() const { return _Parameters; }
	auto GetSize() const { return _Size; }
	auto GetTextureType() const { return _Type; }
	auto ShouldGenerateMipmap() { return _GenMipmap; }
};

class Texture2D : public Texture
{
public:
	virtual ~Texture2D();

	virtual void SetData(void *) = 0;

	static UniquePtr<Texture2D> Create();
};

class TextureCubemap : public Texture
{
public:
	virtual ~TextureCubemap() {}

	virtual void SetFaceData(CubemapFace face, ImageLoader::Image) = 0;

	static UniquePtr<TextureCubemap> Create();
};

}
