#pragma once

#include <opfor/core/base.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Vec4.hpp>
#include <opfor/core/types/Mat3.hpp>
#include <opfor/core/types/Mat4.hpp>

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
    virtual void SetUniform(String const &name, Vec3 value) = 0;
    virtual void SetUniform(String const &name, Vec4 value) = 0;
    virtual void SetUniform(String const &name, Mat3 value) = 0;
    virtual void SetUniform(String const &name, Mat4 value) = 0;
    virtual void SetUniform(String const &name, Vector<Mat4> matrices,
                            Optional<size_t> size = std::nullopt) = 0;
    virtual void SetUniform(String const &name, Vector<Vec3> vectors,
                            Optional<size_t> size = std::nullopt) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    static UniquePtr<Shader> Create();
    static UniquePtr<Shader> Create(String const &source);
};

} // namespace opfor
