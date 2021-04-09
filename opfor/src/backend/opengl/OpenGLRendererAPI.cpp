#include "OpenGLRendererAPI.hpp"

#include "OpenGLConversions.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/renderer/Shader.hpp"
#include "opfor/renderer/Texture.hpp"
#include "opfor/renderer/VertexArray.hpp"

namespace opfor
{

UniquePtr<RendererAPI> RendererAPI::Create()
{
    return MakeUnique<OpenGLRendererAPI>();
}

RendererAPI::Backend RendererAPI::GetBackend() noexcept
{
    return RendererAPI::Backend::OpenGL;
}

void OpenGLRendererAPI::PushViewport(UVec2 pos, UVec2 size)
{
    std::array<GLint, 4> prevViewport{};

    glGetIntegerv(GL_VIEWPORT, prevViewport.data());

    _prevViewports.push_back({{prevViewport[0], prevViewport[1]}, {prevViewport[2], prevViewport[3]}});

    glViewport(pos.x, pos.y, size.x, size.y);
}

void OpenGLRendererAPI::PopViewport()
{
    auto prevViewport = _prevViewports.back();

    glViewport(static_cast<GLint>(prevViewport.first.x), static_cast<GLint>(prevViewport.first.y),
               static_cast<GLsizei>(prevViewport.second.x), static_cast<GLsizei>(prevViewport.second.y));

    _prevViewports.pop_back();
}

static GLbitfield ClearFlagToOpenGLBit(ClearFlag flag)
{
    uint32_t flags = 0;

    if ((flag & ClearFlag::ColorBit) != ClearFlag::None)
    {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    if ((flag & ClearFlag::DepthBit) != ClearFlag::None)
    {
        flags |= GL_DEPTH_BUFFER_BIT;
    }

    return flags;
}

void OpenGLRendererAPI::Clear(ClearFlag flag)
{
    glClear(ClearFlagToOpenGLBit(flag));
}

void OpenGLRendererAPI::SetClearColor(std::array<float, 4> const color)
{
    glClearColor(color[0], color[1], color[2], color[3]);
}

void OpenGLRendererAPI::DrawIndexed(SharedPtr<VertexArray> const &vertexArray)
{
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertexArray->GetIndexBuffer()->GetCount()), GL_UNSIGNED_INT,
                   nullptr);
}

void OpenGLRendererAPI::PushFramebuffer(SharedPtr<Framebuffer> const &fb)
{
    int32_t prevRead = 0;
    int32_t prevDraw = 0;

    // Save previous read/draw buffers
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevRead);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDraw);

    _prevReadBuffers.push_back(prevRead);
    _prevDrawBuffers.push_back(prevDraw);

    glBindFramebuffer(GL_FRAMEBUFFER, fb->GetRawHandle());
}

void OpenGLRendererAPI::PopFramebuffer()
{
    uint32_t prevRead = _prevReadBuffers.back();
    uint32_t prevDraw = _prevDrawBuffers.back();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, prevRead);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDraw);

    _prevDrawBuffers.pop_back();
    _prevReadBuffers.pop_back();
}

void OpenGLRendererAPI::CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &src, CopyTarget target) const
{
    src->CopyToDefault(target);
}

void OpenGLRendererAPI::CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src,
                                                     CopyTarget target) const
{
    src->CopyTo(target, *dst);
}

void OpenGLRendererAPI::CopyDefaultFramebufferTo(SharedPtr<Framebuffer> const &dst, CopyTarget target) const
{
    dst->CopyDefault(target);
}

void OpenGLRendererAPI::PushCapability(RendererCaps cap, bool enable)
{
    if (enable)
    {
        glEnable(ToGlRendererCap(cap));
    }
    else
    {
        glDisable(ToGlRendererCap(cap));
    }

    _capStates[cap].push_back(enable);
}

void OpenGLRendererAPI::PopCapability(RendererCaps cap)
{
    if (_capStates[cap].size() > 0)
    {
        int32_t prevState = _capStates[cap].back();

        if (prevState)
        {
            glEnable(ToGlRendererCap(cap));
        }
        else
        {
            glDisable(ToGlRendererCap(cap));
        }

        _capStates[cap].pop_back();
    }
}

void OpenGLRendererAPI::PushTexture(SharedPtr<Texture> const &texture, TextureUnit unit)
{
    int32_t prevTexture = 0;
    TextureType type = texture->GetTextureType();

    glActiveTexture(ToGlTextureUnit(unit));

    if (type == TextureType::Tex1D)
    {
        glGetIntegerv(GL_TEXTURE_BINDING_1D, &prevTexture);
    }
    else if (type == TextureType::Tex2D)
    {
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    }
    else if (type == TextureType::Tex3D)
    {
        glGetIntegerv(GL_TEXTURE_BINDING_3D, &prevTexture);
    }
    else if (type == TextureType::TexCubemap)
    {
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &prevTexture);
    }

    _prevTextureUnits[unit].push_back({type, prevTexture});

    glBindTexture(ToGlTextureType(type), texture->GetRawHandle());
}

void OpenGLRendererAPI::PopTexture(TextureUnit unit)
{
    auto prevTexture = _prevTextureUnits[unit].back();

    glActiveTexture(ToGlTextureUnit(unit));
    glBindTexture(ToGlTextureType(prevTexture.first), prevTexture.second);

    _prevTextureUnits[unit].pop_back();
}

void OpenGLRendererAPI::PushShader(SharedPtr<Shader> const &shader)
{
    int32_t prevProgram = 0;

    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    _prevShaders.push_back(prevProgram);

    glUseProgram(shader->GetRawHandle());
}

void OpenGLRendererAPI::PopShader()
{
    auto prevShader = _prevShaders.back();

    glUseProgram(prevShader);

    _prevShaders.pop_back();
}

uint32_t OpenGLRendererAPI::FindUniformLocation(String name)
{
    auto currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    auto loc = _UniformLocations[currentProgram].find(name);

    if (loc != _UniformLocations[currentProgram].end())
    {
        return loc->second;
    }
    else
    {

        auto loc = glGetUniformLocation(currentProgram, name.c_str());

        if (loc != -1)
        {
            _UniformLocations[currentProgram][name] = loc;
        }

        return loc;
    }
}

void OpenGLRendererAPI::SetDepthMask(bool val)
{
    glDepthMask(val == true ? GL_TRUE : GL_FALSE);
}

void OpenGLRendererAPI::SetUniform(String const &name, int32_t value)
{
    auto loc = FindUniformLocation(name);
    glUniform1i(loc, value);
}

void OpenGLRendererAPI::SetUniform(String const &name, uint32_t value)
{
    auto loc = FindUniformLocation(name);
    glUniform1ui(loc, value);
}

void OpenGLRendererAPI::SetUniform(String const &name, float value)
{
    auto loc = FindUniformLocation(name);
    glUniform1f(loc, value);
}

void OpenGLRendererAPI::SetUniform(String const &name, Vec3 value)
{
    auto loc = FindUniformLocation(name);
    glUniform3f(loc, value.x, value.y, value.z);
}

void OpenGLRendererAPI::SetUniform(String const &name, Vec4 value)
{
    auto loc = FindUniformLocation(name);
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void OpenGLRendererAPI::SetUniform(String const &name, Mat3 value)
{
    auto loc = FindUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
}

void OpenGLRendererAPI::SetUniform(String const &name, Mat4 value)
{
    auto loc = FindUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}

void OpenGLRendererAPI::SetUniform(String const &name, Vector<Mat4> matrices, Optional<size_t> size)
{
    auto dataSize = matrices.size();

    if (size)
    {
        dataSize = *size;
    }

    auto loc = FindUniformLocation(name);
    glUniformMatrix4fv(loc, static_cast<GLsizei>(dataSize), GL_FALSE, reinterpret_cast<float *>(matrices.data()));
}

void OpenGLRendererAPI::SetUniform(String const &name, Vector<Vec3> vectors, Optional<size_t> size)
{
    auto dataSize = vectors.size();

    if (size)
    {
        dataSize = *size;
    }

    auto loc = FindUniformLocation(name);
    glUniform3fv(loc, static_cast<GLsizei>(dataSize), reinterpret_cast<float *>(vectors.data()));
}

} // namespace opfor
