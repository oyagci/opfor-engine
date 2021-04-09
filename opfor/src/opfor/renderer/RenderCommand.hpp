#pragma once

#include "RendererAPI.hpp"
#include <opfor/core/types/Vec2.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Vec4.hpp>
#include <opfor/core/types/Mat3.hpp>
#include <opfor/core/types/Mat4.hpp>

namespace opfor
{

class RenderCommand
{
  private:
    static UniquePtr<RendererAPI> _API;

  public:
    static inline void PushViewport(UVec2 pos, UVec2 size)
    {
        _API->PushViewport(pos, size);
    }

    static inline void PopViewport()
    {
        _API->PopViewport();
    }

    static inline void SetClearColor(Array<float, 4> const color)
    {
        _API->SetClearColor(std::forward<decltype(color)>(color));
    }

    static inline void Clear(ClearFlag flag)
    {
        _API->Clear(flag);
    }

    static inline void DrawIndexed(SharedPtr<VertexArray> const &vertexArray)
    {
        _API->DrawIndexed(std::forward<decltype(vertexArray)>(vertexArray));
    }

    static inline void PushFramebuffer(SharedPtr<Framebuffer> const &fb)
    {
        _API->PushFramebuffer(fb);
    }

    static inline void PopFramebuffer()
    {
        _API->PopFramebuffer();
    }

    static inline void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> fb, CopyTarget target)
    {
        _API->CopyFramebufferToDefaultFramebuffer(fb, target);
    }

    static inline void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src,
                                                    CopyTarget target)
    {
        _API->CopyFramebufferToFramebuffer(dst, src, target);
    }

    static inline void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget target)
    {
        _API->CopyDefaultFramebufferTo(dst, target);
    }

    static inline void PushCapability(RendererCaps cap, bool enable)
    {
        _API->PushCapability(cap, enable);
    }

    static inline void PopCapability(RendererCaps cap)
    {
        _API->PopCapability(cap);
    }

    static inline void PushTexture(SharedPtr<Texture> texture, TextureUnit unit)
    {
        _API->PushTexture(texture, unit);
    }

    static inline void PopTexture(TextureUnit unit)
    {
        _API->PopTexture(unit);
    }

    static inline void PushShader(SharedPtr<Shader> shader)
    {
        _API->PushShader(shader);
    }

    static inline void PopShader()
    {
        _API->PopShader();
    }

    static inline void SetUniform(String const &name, int32_t value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, uint32_t value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, float value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, Vec3 value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, Vec4 value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, Mat3 value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, Mat4 value)
    {
        _API->SetUniform(name, value);
    }

    static inline void SetUniform(String const &name, std::vector<Mat4> matrices,
                                  Optional<size_t> size = std::nullopt)
    {
        _API->SetUniform(name, matrices, size);
    }

    static inline void SetUniform(String const &name, std::vector<Vec3> vectors,
                                  Optional<size_t> size = std::nullopt)
    {
        _API->SetUniform(name, vectors, size);
    }

    static inline void SetDepthMask(bool val)
    {
        _API->SetDepthMask(val);
    }
};

} // namespace opfor
