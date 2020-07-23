#pragma once

#include "engine/renderer/Shader.hpp"

namespace opfor {

class OpenGLShader : public Shader
{
private:
	uint32_t _RendererID;

	std::optional<uint32_t> _GeometryShader;
	std::optional<uint32_t> _VertexShader;
	std::optional<uint32_t> _FragmentShader;

	std::optional<std::string> ReadShaderSource(std::string const &);

	std::optional<uint32_t> CreateShader(std::string const &src, uint32_t shaderType);

	std::unordered_map<std::string, uint32_t> _UniformLocations;

	uint32_t FindUniformLocation(std::string);

public:
	OpenGLShader();
	virtual ~OpenGLShader();

	void AddGeometryShader(std::string const &) override;
	void AddVertexShader(std::string const &) override;
	void AddFragmentShader(std::string const &) override;

	void Link() override;

	void SetUniform(std::string const &name, size_t value) override;
	void SetUniform(std::string const &name, int32_t value) override;
	void SetUniform(std::string const &name, uint32_t value) override;
	void SetUniform(std::string const &name, float value) override;
	void SetUniform(std::string const &name, glm::vec3 value) override;
	void SetUniform(std::string const &name, glm::vec4 value) override;
	void SetUniform(std::string const &name, glm::mat3 value) override;
	void SetUniform(std::string const &name, glm::mat4 value) override;
	void SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size = std::nullopt) override;
	void SetUniform(std::string const &name, std::vector<glm::vec3> matrices, std::optional<size_t> size = std::nullopt) override;

	uint32_t GetRawHandle() const override { return _RendererID; }

	void Bind() override;
	void Unbind() override;
};

}
