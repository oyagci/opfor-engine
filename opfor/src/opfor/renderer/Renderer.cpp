#include "Renderer.hpp"

#include "opfor/core/base.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/VertexArray.hpp"
#include <unordered_map>

#if defined(OP4_PLATFORM_LINUX)
#define __OP4_FUNCNAME__ __PRETTY_FUNCTION__
#elif defined(OP4_PLATFORM_WINDOWS)
#define __OP4_FUNCNAME__ __FUNCTION__
#endif

namespace opfor
{

List<std::function<void()>> Renderer::_Calls;
Vector<String> _CallsStrings;

void Renderer::PrintTree(unsigned int offset)
{
    fmt::print("=====================================================\n");
    for (auto const &s : _CallsStrings)
    {
        fmt::print("{}\n", s);
    }
}

template <typename FuncType, typename... ArgTypes> static inline constexpr auto BIND(FuncType fn, ArgTypes &&...args)
{
    return std::bind(fn, std::forward<ArgTypes>(args)...);
}

ClearFlag operator|(ClearFlag lhs, ClearFlag rhs)
{
    return static_cast<ClearFlag>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

ClearFlag operator&(ClearFlag lhs, ClearFlag rhs)
{
    return static_cast<ClearFlag>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

void Renderer::BeginScene()
{
    _Calls.clear();
}

void Renderer::EndScene()
{
    // PrintTree();
    _CallsStrings.clear();

    while (!_OptimizedRenderCommands.empty())
    {
        auto const &nextRenderCommand = _OptimizedRenderCommands.front();

        ExecRenderCommandBuffer(nextRenderCommand);

        _OptimizedRenderCommands.pop_front();
    }

    while (!_Calls.empty())
    {
        auto &nextCall = _Calls.front();

        nextCall();

        _Calls.pop_front();
    }
}

void Renderer::PushViewport(UVec2 pos, UVec2 size)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PushViewport, pos, size));
}

void Renderer::PopViewport()
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PopViewport));
}

void Renderer::SetClearColor(Array<float, 4> const &color)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::SetClearColor, std::forward<decltype(color)>(color)));
}

void Renderer::Clear(ClearFlag flag)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::Clear, flag));
}

void Renderer::Submit(SharedPtr<VertexArray> const &vertexArray)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([vertexArray]() {
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
        vertexArray->Unbind();
    });
}

void Renderer::Submit(const std::function<void()> &f)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(f);
}

void Renderer::SubmitDrawCommand(DrawCommand const &drawCommand)
{

    for (auto const &t : drawCommand.textureBindings)
    {
        Renderer::PushTexture(t.texture, t.binding);
    }

    for (auto const &binding : drawCommand.uniformBindings)
    {

        if (std::holds_alternative<int>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<int>(binding.value));
        }
        //		else if (std::holds_alternative<unsigned int>(binding.value)) {
        //			Renderer::Shader::SetUniform(binding.name, std::get<unsigned int>(binding.value));
        //		}
        else if (std::holds_alternative<float>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<float>(binding.value));
        }
        else if (std::holds_alternative<Vec3>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<Vec3>(binding.value));
        }
        else if (std::holds_alternative<Vec4>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<Vec4>(binding.value));
        }
        //		else if (std::holds_alternative<Mat3>(binding.value)) {
        //			//Renderer::Shader::SetUniform(binding.name, std::get<Mat3>(binding.value));
        //			throw std::runtime_error("Unsupported uniform binding type");
        //		}
        else if (std::holds_alternative<Mat4>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<Mat4>(binding.value));
        }
        else if (std::holds_alternative<Vector<Mat4>>(binding.value))
        {
            Renderer::Shader::SetUniform(binding.name, std::get<Vector<Mat4>>(binding.value));
        }
        else
        {
            throw std::runtime_error("Unsupported uniform binding type");
        }
    }

    Renderer::Submit(drawCommand.vertexArray);

    for (auto const &t : drawCommand.textureBindings)
    {
        Renderer::PopTexture(t.binding);
    }
}

void Renderer::SubmitRenderCommandBuffer(RenderCommandBuffer const &renderCommand)
{
    // For each RenderCommandBuffer:
    //   - Get a list of DrawCommands which are using the same shaders
    //   - Sort the draw commands by their shader
    //   - Create a RenderCommandBufferOptimized using the sorted list of DrawCommands

    UnorderedMap<SharedPtr<opfor::Shader>, Vector<DrawCommand>> drawCommandsByShader;
    for (auto const &dc : renderCommand.drawCommands)
    {
        drawCommandsByShader[dc.shader].push_back(dc);
    }

    RenderCommandBufferOptimized optimizedRenderCommand;
    optimizedRenderCommand.framebuffer = renderCommand.framebuffer;
    optimizedRenderCommand.viewportExtent = renderCommand.viewportExtent;
    optimizedRenderCommand.clear = renderCommand.clear;
    optimizedRenderCommand.capabilities = renderCommand.capabilities;
    optimizedRenderCommand.drawCommandsByShader = std::move(drawCommandsByShader);
    optimizedRenderCommand.disableDepthMask = renderCommand.disableDepthMask;

    ExecRenderCommandBuffer(optimizedRenderCommand);
    //_OptimizedRenderCommands.push_back(std::move(optimizedRenderCommand));
}

void Renderer::ExecRenderCommandBuffer(RenderCommandBufferOptimized const &renderCommand)
{
    for (auto const &cap : renderCommand.capabilities)
    {
        Renderer::PushCapability(cap.first, cap.second);
    }

    if (renderCommand.framebuffer)
    {
        Renderer::PushFramebuffer(*renderCommand.framebuffer);
    }

    if (renderCommand.viewportExtent)
    {
        Renderer::PushViewport(renderCommand.viewportExtent->first, renderCommand.viewportExtent->second);
    }

    if (renderCommand.clear)
    {
        Renderer::SetClearColor(renderCommand.clear->color);
        Renderer::Clear(renderCommand.clear->flags);
    }

    if (renderCommand.disableDepthMask)
    {
        glDepthMask(false);
    }

    // Draw
    // ====
    for (auto const &[shader, uniqueShaderCommands] : renderCommand.drawCommandsByShader)
    {
        Renderer::Shader::Push(shader);
        for (auto const &drawCommand : uniqueShaderCommands)
        {
            Renderer::SubmitDrawCommand(drawCommand);
        }
        Renderer::Shader::Pop();
    }

    // Cleanup
    // =======

    if (renderCommand.disableDepthMask)
    {
        glDepthMask(true);
    }

    if (renderCommand.viewportExtent)
    {
        Renderer::PopViewport();
    }

    if (renderCommand.framebuffer)
    {
        Renderer::PopFramebuffer();
    }

    for (auto const &cap : renderCommand.capabilities)
    {
        Renderer::PopCapability(cap.first);
    }
}

void Renderer::PushFramebuffer(SharedPtr<Framebuffer> fb)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PushFramebuffer, fb));
}

void Renderer::PopFramebuffer()
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PopFramebuffer));
}

void Renderer::PushCapability(RendererCaps cap, bool enable)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PushCapability, cap, enable));
}

void Renderer::PopCapability(RendererCaps cap)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PopCapability, cap));
}

void Renderer::CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> dst, SharedPtr<Framebuffer> src, CopyTarget target)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::CopyFramebufferToFramebuffer, dst, src, target));
}

void Renderer::CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> src, CopyTarget target)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::CopyFramebufferToDefaultFramebuffer, src, target));
}

void Renderer::CopyDefaultFramebufferTo(SharedPtr<Framebuffer> dst, CopyTarget target)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::CopyDefaultFramebufferTo, dst, target));
}

void Renderer::PushTexture(SharedPtr<Texture> texture, TextureUnit unit)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PushTexture, texture, unit));
}

void Renderer::PopTexture(TextureUnit unit)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PopTexture, unit));
}

void Renderer::SetDepthMask(bool val)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::SetDepthMask, val));
}

// Shader
// ======

void Renderer::Shader::Push(SharedPtr<opfor::Shader> shader)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PushShader, shader));
}

void Renderer::Shader::Pop()
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back(BIND(RenderCommand::PopShader));
}

void Renderer::Shader::SetUniform(String const &name, size_t value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, static_cast<uint32_t>(value)); });
}

void Renderer::Shader::SetUniform(String const &name, int32_t value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, uint32_t value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, float value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, Vec3 value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, Vec4 value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, Mat3 value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, Mat4 value)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, value]() { RenderCommand::SetUniform(name, value); });
}

void Renderer::Shader::SetUniform(String const &name, Vector<Mat4> matrices, Optional<size_t> size)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, matrices, size]() { RenderCommand::SetUniform(name, matrices, size); });
}

void Renderer::Shader::SetUniform(String const &name, Vector<Vec3> vectors, Optional<size_t> size)
{
    _CallsStrings.push_back(__OP4_FUNCNAME__);
    _Calls.push_back([name, vectors, size]() { RenderCommand::SetUniform(name, vectors, size); });
}

} // namespace opfor
