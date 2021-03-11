#pragma once

#include <opfor/core/base.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Renderbuffer.hpp>
#include <renderer/Renderer.hpp>
#include <renderer/Texture.hpp>

namespace opfor
{

inline GLenum ToGlColorAttachmentEnum(FramebufferAttachment attachment)
{
    static const UnorderedMap<FramebufferAttachment, GLenum> lookup = {
        {FramebufferAttachment::ColorAttachment0, GL_COLOR_ATTACHMENT0},
        {FramebufferAttachment::ColorAttachment1, GL_COLOR_ATTACHMENT1},
        {FramebufferAttachment::ColorAttachment2, GL_COLOR_ATTACHMENT2},
        {FramebufferAttachment::ColorAttachment3, GL_COLOR_ATTACHMENT3},
        {FramebufferAttachment::ColorAttachment4, GL_COLOR_ATTACHMENT4},
        {FramebufferAttachment::ColorAttachment5, GL_COLOR_ATTACHMENT5},
        {FramebufferAttachment::ColorAttachment6, GL_COLOR_ATTACHMENT6},
        {FramebufferAttachment::ColorAttachment7, GL_COLOR_ATTACHMENT7},
        {FramebufferAttachment::ColorAttachment8, GL_COLOR_ATTACHMENT8},
        {FramebufferAttachment::ColorAttachment9, GL_COLOR_ATTACHMENT9},
        {FramebufferAttachment::ColorAttachment10, GL_COLOR_ATTACHMENT10},
        {FramebufferAttachment::ColorAttachment11, GL_COLOR_ATTACHMENT11},
        {FramebufferAttachment::ColorAttachment12, GL_COLOR_ATTACHMENT12},
        {FramebufferAttachment::ColorAttachment13, GL_COLOR_ATTACHMENT13},
        {FramebufferAttachment::ColorAttachment14, GL_COLOR_ATTACHMENT14},
        {FramebufferAttachment::ColorAttachment15, GL_COLOR_ATTACHMENT15},
        {FramebufferAttachment::DepthAttachment, GL_DEPTH_ATTACHMENT},
    };

    return lookup.at(attachment);
}

constexpr GLenum ToGlRenderbufferAttachment(RenderbufferAttachment attachment)
{
    switch (attachment)
    {
    case RenderbufferAttachment::DepthAttachment:
        return GL_DEPTH_ATTACHMENT;
    case RenderbufferAttachment::DepthStencilAttachment:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    default:
        OP4_CORE_UNREACHABLE("Invalid renderbuffer attachment");
    }
}

constexpr GLenum ToGlBufferBit(CopyTarget target)
{
    switch (target)
    {
    case CopyTarget::ColorBufferBit:
        return GL_COLOR_BUFFER_BIT;
    case CopyTarget::DepthBufferBit:
        return GL_DEPTH_BUFFER_BIT;
    case CopyTarget::StencilBufferBit:
        return GL_STENCIL_BUFFER_BIT;
    default:
        OP4_CORE_UNREACHABLE("Invalid CopyTarget");
    }
}

constexpr GLenum ToGlRendererCap(RendererCaps cap)
{
    switch (cap)
    {
    case RendererCaps::Blend:
        return GL_BLEND;
    case RendererCaps::CullFace:
        return GL_CULL_FACE;
    case RendererCaps::DepthTest:
        return GL_DEPTH_TEST;
    default:
        OP4_CORE_UNREACHABLE("Invalid Renderer capability");
    }
}

constexpr GLenum ToGlTextureType(TextureType textureType)
{
    switch (textureType)
    {
    case TextureType::Tex1D:
        return GL_TEXTURE_1D;
    case TextureType::Tex2D:
        return GL_TEXTURE_2D;
    case TextureType::Tex3D:
        return GL_TEXTURE_3D;
    case TextureType::TexCubemap:
        return GL_TEXTURE_CUBE_MAP;
    default:
        OP4_CORE_UNREACHABLE("Invalid texture type");
    }
}

constexpr GLint ToGlDataFormat(DataFormat dataFormat)
{
    switch (dataFormat)
    {
    case DataFormat::RG:
        return GL_RG;
    case DataFormat::RGB:
        return GL_RGB;
    case DataFormat::RGBA:
        return GL_RGBA;
    case DataFormat::RGBA16F:
        return GL_RGBA16F;
    case DataFormat::RGB16F:
        return GL_RGB16F;
    case DataFormat::Depth:
        return GL_DEPTH_COMPONENT;
    default:
        OP4_CORE_UNREACHABLE("Invalid data format");
    }
}

constexpr GLenum ToGlDataType(DataType dataType)
{
    switch (dataType)
    {
    case DataType::Int:
        return GL_INT;
    case DataType::Float:
        return GL_FLOAT;
    case DataType::UnsignedByte:
        return GL_UNSIGNED_BYTE;
    case DataType::UnsignedInt:
        return GL_UNSIGNED_INT;
    default:
        OP4_CORE_UNREACHABLE("Invalid data type");
    }
}

constexpr GLenum ToGlTextureParameterType(TextureParameterType textureType)
{
    switch (textureType)
    {
    case TextureParameterType::MignifyFilter:
        return GL_TEXTURE_MIN_FILTER;
    case TextureParameterType::MagnifyFilter:
        return GL_TEXTURE_MAG_FILTER;
    case TextureParameterType::WrapR:
        return GL_TEXTURE_WRAP_R;
    case TextureParameterType::WrapS:
        return GL_TEXTURE_WRAP_S;
    case TextureParameterType::WrapT:
        return GL_TEXTURE_WRAP_T;
    default:
        OP4_CORE_UNREACHABLE("Invalid texture parameter type");
    }
}

constexpr GLint ToGlTextureParameterValue(TextureParameterValue value)
{
    switch (value)
    {
    case TextureParameterValue::Nearest:
        return GL_NEAREST;
    case TextureParameterValue::Linear:
        return GL_LINEAR;
    case TextureParameterValue::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case TextureParameterValue::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case TextureParameterValue::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case TextureParameterValue::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case TextureParameterValue::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case TextureParameterValue::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case TextureParameterValue::Repeat:
        return GL_REPEAT;
    case TextureParameterValue::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    default:
        OP4_CORE_UNREACHABLE("Invalid texture parameter value");
    }
}

constexpr GLenum ToGlCubemapFace(CubemapFace face)
{
    switch (face)
    {
    case CubemapFace::PositiveX:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case CubemapFace::NegativeX:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case CubemapFace::PositiveY:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case CubemapFace::NegativeY:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case CubemapFace::PositiveZ:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case CubemapFace::NegativeZ:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    default:
        OP4_CORE_UNREACHABLE("Invalid cubemap face");
    }
}

constexpr GLenum ToGlTextureUnit(TextureUnit textureUnit)
{
    static_assert(static_cast<int>(TextureUnit::Texture0) == 0);
    return GL_TEXTURE0 + static_cast<int>(textureUnit);
}

} // namespace opfor
