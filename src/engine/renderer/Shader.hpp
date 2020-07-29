#pragma once

#include "engine/core/base.hpp"
#include <glm/glm.hpp>
#include <array>

namespace opfor {

class Shader
{
public:
	virtual ~Shader() {}

	virtual uint32_t GetRawHandle() const = 0;

	virtual void SetUniform(std::string const &name, size_t value) = 0;
	virtual void SetUniform(std::string const &name, int32_t value) = 0;
	virtual void SetUniform(std::string const &name, uint32_t value) = 0;
	virtual void SetUniform(std::string const &name, float value) = 0;
	virtual void SetUniform(std::string const &name, glm::vec3 value) = 0;
	virtual void SetUniform(std::string const &name, glm::vec4 value) = 0;
	virtual void SetUniform(std::string const &name, glm::mat3 value) = 0;
	virtual void SetUniform(std::string const &name, glm::mat4 value) = 0;
	virtual void SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size = std::nullopt) = 0;
	virtual void SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size = std::nullopt) = 0;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	static UniquePtr<Shader> Create();
	static UniquePtr<Shader> Create(std::string const &source);
};

}
