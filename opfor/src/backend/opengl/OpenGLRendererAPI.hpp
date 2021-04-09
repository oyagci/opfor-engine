#pragma once

#include "opfor/renderer/RendererAPI.hpp"

namespace opfor
{

enum class TextureType;

class OpenGLRendererAPI : public RendererAPI
{
  private:
    Vector<uint32_t> _prevReadBuffers;
    Vector<uint32_t> _prevDrawBuffers;

    using CapState = std::unordered_map<RendererCaps, Vector<bool>>;
    CapState _capStates;

    using TextureUnits = std::unordered_map<TextureUnit, Vector<std::pair<TextureType, uint32_t>>>;
    TextureUnits _prevTextureUnits;

    Vector<uint32_t> _prevShaders;

    using ProgramID = uint32_t;
    using UniformName = String;
    using UniformLocations = std::unordered_map<UniformName, uint32_t>;
    using ShaderUniformLocations = std::unordered_map<ProgramID, UniformLocations>;

    ShaderUniformLocations _UniformLocations;

    Vector<std::pair<Vec2, Vec2>> _prevViewports;

    uint32_t FindUniformLocation(String name);

  public:
    void PushViewport(UVec2 pos, UVec2 size) override;
    void PopViewport() override;

    void Clear(ClearFlag) override;
    void SetClearColor(std::array<float, 4> const) override;

    void DrawIndexed(SharedPtr<VertexArray> const &) override;

    void PushFramebuffer(SharedPtr<Framebuffer> const &) override;
    void PopFramebuffer() override;

    void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const override;
    void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src,
                                      CopyTarget) const override;
    void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> const &, CopyTarget) const override;

    void PushCapability(RendererCaps, bool enable) override;
    void PopCapability(RendererCaps) override;

    void PushTexture(SharedPtr<Texture> const &, TextureUnit) override;
    void PopTexture(TextureUnit) override;

    void PushShader(SharedPtr<Shader> const &) override;
    void PopShader() override;

    void SetDepthMask(bool val) override;

    void SetUniform(String const &name, int32_t value) override;
    void SetUniform(String const &name, uint32_t value) override;
    void SetUniform(String const &name, float value) override;
    void SetUniform(String const &name, Vec3 value) override;
    void SetUniform(String const &name, Vec4 value) override;
    void SetUniform(String const &name, Mat3 value) override;
    void SetUniform(String const &name, Mat4 value) override;
    void SetUniform(String const &name, Vector<Mat4> matrices,
                    Optional<size_t> size = std::nullopt) override;
    void SetUniform(String const &name, Vector<Vec3> vectors,
                    Optional<size_t> size = std::nullopt) override;

};

} // namespace opfor
