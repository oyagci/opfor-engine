#pragma once

#include "opfor/core/base.hpp"
#include <opfor/core/types/Vec2.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Vec4.hpp>
#include <opfor/core/types/Mat3.hpp>
#include <opfor/core/types/Mat4.hpp>

namespace opfor
{

// Forward Declarations
// ====================
class VertexArray;
class Framebuffer;
class Texture;
class Shader;

enum class RendererCaps;
enum class CopyTarget;
enum class TextureUnit;
enum class ClearFlag;

class RendererAPI
{
  public:
    enum class Backend
    {
        OpenGL = 0,
        Vulkan = 1,
    };

  public:
    virtual ~RendererAPI() = default;

    virtual void PushViewport(UVec2 pos, UVec2 size) = 0;
    virtual void PopViewport() = 0;

    virtual void Clear(ClearFlag) = 0;
    virtual void SetClearColor(std::array<float, 4>) = 0;

    virtual void DrawIndexed(SharedPtr<VertexArray> const &) = 0;

    virtual void PushFramebuffer(SharedPtr<Framebuffer> const &) = 0;
    virtual void PopFramebuffer() = 0;

    virtual void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const = 0;
    virtual void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src,
                                              CopyTarget) const = 0;
    virtual void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> const &, CopyTarget) const = 0;

    virtual void PushCapability(RendererCaps, bool enable) = 0;
    virtual void PopCapability(RendererCaps) = 0;

    virtual void PushTexture(SharedPtr<Texture> const &, TextureUnit) = 0;
    virtual void PopTexture(TextureUnit) = 0;

    virtual void PushShader(SharedPtr<Shader> const &) = 0;
    virtual void PopShader() = 0;

    virtual void SetUniform(String const &name, int32_t value) = 0;
    virtual void SetUniform(String const &name, uint32_t value) = 0;
    virtual void SetUniform(String const &name, float value) = 0;
    virtual void SetUniform(String const &name, Vec3 value) = 0;
    virtual void SetUniform(String const &name, Vec4 value) = 0;
    virtual void SetUniform(String const &name, Mat3 value) = 0;
    virtual void SetUniform(String const &name, Mat4 value) = 0;
    virtual void SetUniform(String const &name, Vector<Mat4> matrices,
                            Optional<size_t> size = std::nullopt) = 0;
    virtual void SetUniform(String const &name, Vector<Vec3> vectors,
                            Optional<size_t> size = std::nullopt) = 0;

    virtual void SetDepthMask(bool val) = 0;

    static Backend GetBackend() noexcept;
    static UniquePtr<RendererAPI> Create();
};

} // namespace opfor
