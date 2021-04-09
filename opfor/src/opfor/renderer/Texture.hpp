#pragma once

#include <opfor/core/ImageLoader.hpp>
#include <opfor/core/base.hpp>
#include <opfor/core/types/Vec3.hpp>
#include "renderer.hpp"

namespace opfor
{

enum class CubemapFace
{
    PositiveX = 1,
    NegativeX = 2,
    PositiveY = 3,
    NegativeY = 4,
    PositiveZ = 5,
    NegativeZ = 6,
};

enum class TextureType
{
    Tex1D = 1,
    Tex2D = 2,
    Tex3D = 3,
    TexCubemap = 4,
};

enum class DataFormat
{
    RG = 1,
    RGB = 2,
    RGBA = 3,
    RGBA16F = 4,
    RGB16F = 5,
    Depth = 6,
};

enum class DataType
{
    Int = 1,
    Float = 2,
    UnsignedByte = 3,
    UnsignedInt = 4,
};

enum class TextureParameterType
{
    MignifyFilter = 1,
    MagnifyFilter = 2,
    WrapR = 3,
    WrapS = 4,
    WrapT = 5,
};

enum class TextureParameterValue
{
    Nearest = 1,
    Linear = 2,
    NearestMipmapNearest = 3,
    NearestMipmapLinear = 4,
    LinearMipmapNearest = 5,
    LinearMipmapLinear = 6,
    ClampToEdge = 7,
    ClampToBorder = 8,
    Repeat = 9,
    MirroredRepeat = 10,
};

enum class TextureUnit
{
    Texture0 = 0,
    Texture1 = 1,
    Texture2 = 2,
    Texture3 = 3,
    Texture4 = 4,
    Texture5 = 5,
    Texture6 = 6,
    Texture7 = 7,
    Texture8 = 8,
    Texture9 = 9,
    Texture10 = 10,
    Texture11 = 11,
    Texture12 = 12,
    Texture13 = 13,
    Texture14 = 14,
    Texture15 = 15,
};

using TextureParameter = std::pair<TextureParameterType, TextureParameterValue>;
using TextureParameterList = Vector<TextureParameter>;

class Texture
{
  private:
    DataFormat _InputFormat = DataFormat::RGB;
    DataFormat _OutputFormat = DataFormat::RGB;
    DataType _DataType = DataType::Int;
    TextureParameterList _Parameters;
    UVec3 _Size;

    bool _HasAlpha = false;
    bool _IsSRGB = false;
    bool _GenMipmap = false;

    TextureType _Type = TextureType::Tex2D;

  protected:
    void SetTextureType(TextureType type)
    {
        _Type = type;
    }

  public:
    virtual ~Texture() = default;

    virtual void Bind(TextureUnit) = 0;

    virtual uint32_t GetRawHandle() const = 0;

    virtual void Build() = 0;

    void SetInputFormat(DataFormat input)
    {
        _InputFormat = input;
    }
    void SetOutputFormat(DataFormat output)
    {
        _OutputFormat = output;
    }
    void SetDataType(DataType dataType)
    {
        _DataType = dataType;
    }
    void SetHasAlpha(bool alpha)
    {
        _HasAlpha = alpha;
    }
    void SetIsSRGB(bool srgb)
    {
        _IsSRGB = srgb;
    }
    void SetTextureParameters(TextureParameterList paramList)
    {
        _Parameters = paramList;
    }
    void SetSize(unsigned int width, unsigned int height = 0, unsigned int depth = 0)
    {
        _Size.x = width;
        _Size.y = height;
        _Size.z = depth;
    }
    void SetGenerateMipmap(bool val)
    {
        _GenMipmap = val;
    }

    auto GetInputFormat() const
    {
        return _InputFormat;
    }
    auto GetOutputFormat() const
    {
        return _OutputFormat;
    }
    auto GetDataType() const
    {
        return _DataType;
    }
    auto HasAlpha() const
    {
        return _HasAlpha;
    }
    auto IsSRGB() const
    {
        return _IsSRGB;
    }
    auto const &GetTextureParameters() const
    {
        return _Parameters;
    }
    auto GetSize() const
    {
        return _Size;
    }
    auto GetTextureType() const
    {
        return _Type;
    }
    auto ShouldGenerateMipmap()
    {
        return _GenMipmap;
    }
};

class Texture2D : public Texture
{
  public:
    virtual ~Texture2D() = default;

    virtual void SetData(void *) = 0;

    static UniquePtr<Texture2D> Create();
};

class TextureCubemap : public Texture
{
  public:
    virtual ~TextureCubemap() = default;

    virtual void SetFaceData(CubemapFace face, ImageLoader::Image) = 0;

    static UniquePtr<TextureCubemap> Create();
};

} // namespace opfor
