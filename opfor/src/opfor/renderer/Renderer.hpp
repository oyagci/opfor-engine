#pragma once

#include "RenderCommand.hpp"
#include "opfor/core/base.hpp"
#include <opfor/core/types/Vec2.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Vec4.hpp>
#include <opfor/core/types/Mat3.hpp>
#include <opfor/core/types/Mat4.hpp>

namespace opfor
{

enum class ClearFlag
{
    None = 0,
    ColorBit = 1 << 0,
    DepthBit = 1 << 1,
};

ClearFlag operator|(ClearFlag lhs, ClearFlag rhs);
ClearFlag operator&(ClearFlag lhs, ClearFlag rhs);

enum class RendererCaps
{
    DepthTest = 1,
    CullFace = 2,
    Blend = 3,
};

struct DrawCommandTextureBinding
{
    SharedPtr<Texture> texture;
    TextureUnit binding;
};

struct UniformBinding
{
    using BindingValue = Variant<int, float, Vec3, Vec4, Mat4, Vector<Mat4>>;

    String name;
    BindingValue value;
};

struct ClearScreenParam
{
    Array<float, 4> color;
    ClearFlag flags;
};

struct DrawCommand
{
    SharedPtr<opfor::Shader> shader;
    SharedPtr<VertexArray> vertexArray;
    Vector<DrawCommandTextureBinding> textureBindings;
    Vector<UniformBinding> uniformBindings;
};

struct RenderCommandBuffer
{
    Optional<SharedPtr<Framebuffer>> framebuffer;
    Optional<Pair<UVec2, UVec2>> viewportExtent;
    Optional<ClearScreenParam> clear;
    Vector<Pair<RendererCaps, bool>> capabilities;
    Vector<DrawCommand> drawCommands;
    bool disableDepthMask;
};

struct RenderCommandBufferOptimized
{
    Optional<SharedPtr<Framebuffer>> framebuffer;
    Optional<Pair<UVec2, UVec2>> viewportExtent;
    Optional<ClearScreenParam> clear;
    Vector<Pair<RendererCaps, bool>> capabilities;
    UnorderedMap<SharedPtr<opfor::Shader>, Vector<DrawCommand>> drawCommandsByShader;
    bool disableDepthMask;
};

enum class RendererCaps;

class Renderer
{
  private:
    static List<std::function<void()>> _Calls;

    static inline List<RenderCommandBufferOptimized> _OptimizedRenderCommands;

    static void PrintTree(unsigned int offset = 0);

  public:
    static void BeginScene();
    static void EndScene();

    static void SetDepthMask(bool);

    static void PushViewport(UVec2 pos, UVec2 size);
    static void PopViewport();

    static void SetClearColor(Array<float, 4> const &color);
    static void Clear(ClearFlag);

    static void Submit(SharedPtr<VertexArray> const &);
    static void Submit(std::function<void()> const &);

    static void PushFramebuffer(SharedPtr<Framebuffer>);
    static void PopFramebuffer();

    static void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget);
    static void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget);
    static void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget);

    static void PushCapability(RendererCaps, bool);
    static void PopCapability(RendererCaps);

    static void PushTexture(SharedPtr<Texture>, TextureUnit);
    static void PopTexture(TextureUnit);

    static RendererAPI::Backend GetBackend()
    {
        return RendererAPI::GetBackend();
    }

    static void SubmitDrawCommand(DrawCommand const &drawCommand);
    static void SubmitRenderCommandBuffer(RenderCommandBuffer const &renderCommand);

    static void ExecRenderCommandBuffer(RenderCommandBufferOptimized const &renderCommand);

    class Shader
    {
      public:
        static void Push(SharedPtr<opfor::Shader>);
        static void Pop();

        static void SetUniform(String const &name, size_t value);
        static void SetUniform(String const &name, int32_t value);
        static void SetUniform(String const &name, uint32_t value);
        static void SetUniform(String const &name, float value);
        static void SetUniform(String const &name, Vec3 value);
        static void SetUniform(String const &name, Vec4 value);
        static void SetUniform(String const &name, Mat3 value);
        static void SetUniform(String const &name, Mat4 value);
        static void SetUniform(String const &name, Vector<Mat4> matrices,
                               Optional<size_t> size = std::nullopt);
        static void SetUniform(String const &name, Vector<Vec3> vectors,
                               Optional<size_t> size = std::nullopt);
    };
};

} // namespace opfor
