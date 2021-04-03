#pragma once

#include <opfor/core/base.hpp>
#include <glm/glm.hpp>

namespace opfor
{

class Shader
{
  public:
    virtual ~Shader() = default;

    virtual uint32_t GetRawHandle() const = 0;

    virtual void SetUniform(String const &name, int32_t value) = 0;
    virtual void SetUniform(String const &name, uint32_t value) = 0;
    virtual void SetUniform(String const &name, float value) = 0;
    virtual void SetUniform(String const &name, glm::vec3 value) = 0;
    virtual void SetUniform(String const &name, glm::vec4 value) = 0;
    virtual void SetUniform(String const &name, glm::mat3 value) = 0;
    virtual void SetUniform(String const &name, glm::mat4 value) = 0;
    virtual void SetUniform(String const &name, std::vector<glm::mat4> matrices,
                            Optional<size_t> size = std::nullopt) = 0;
    virtual void SetUniform(String const &name, std::vector<glm::vec3> vectors,
                            Optional<size_t> size = std::nullopt) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    static UniquePtr<Shader> Create();
    static UniquePtr<Shader> Create(String const &source);
};

} // namespace opfor
