#pragma once

#include <opfor/core/base.hpp>
#include <opfor/renderer/Shader.hpp>

namespace opfor
{

class OpenGLShader : public Shader
{
  private:
    uint32_t _RendererID;

    Optional<uint32_t> _GeometryShader;
    Optional<uint32_t> _VertexShader;
    Optional<uint32_t> _FragmentShader;

    Optional<String> ReadShaderSource(String const &);

    Optional<uint32_t> AddShaderFromPath(String const &path, uint32_t shaderType);
    Optional<uint32_t> AddShaderFromSource(String const &src, uint32_t shaderType);

    UnorderedMap<String, uint32_t> _UniformLocations;

    uint32_t FindUniformLocation(String);

    enum class ShaderParserState
    {
        Normal = 1,
        PreprocessorStart,
        PreprocessorKeyword,
        PreprocessorStop,
    };
    enum class ShaderType
    {
        Geometry = 1,
        Vertex,
        Fragment,
        Compute,
    };
    UnorderedMap<ShaderType, String> ParseShaderSource(String const &src);

    void Link();

  public:
    OpenGLShader(String shaderPath);
    virtual ~OpenGLShader();

    void SetUniform(String const &name, int32_t value) override;
    void SetUniform(String const &name, uint32_t value) override;
    void SetUniform(String const &name, float value) override;
    void SetUniform(String const &name, Vec3 value) override;
    void SetUniform(String const &name, Vec4 value) override;
    void SetUniform(String const &name, Mat3 value) override;
    void SetUniform(String const &name, Mat4 value) override;
    void SetUniform(String const &name, Vector<Mat4> matrices,
                    Optional<size_t> size = std::nullopt) override;
    void SetUniform(String const &name, Vector<Vec3> matrices,
                    Optional<size_t> size = std::nullopt) override;

    uint32_t GetRawHandle() const override
    {
        return _RendererID;
    }

    void Bind() override;
    void Unbind() override;
};

} // namespace opfor
