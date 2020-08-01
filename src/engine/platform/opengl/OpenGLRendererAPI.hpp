#pragma once

#include "engine/renderer/RendererAPI.hpp"

namespace opfor {

enum class TextureType;

class OpenGLRendererAPI : public RendererAPI
{
private:
	std::vector<uint32_t> _prevReadBuffers;
	std::vector<uint32_t> _prevDrawBuffers;

	using CapState = std::unordered_map<RendererCaps, std::vector<bool>>;
	CapState _capStates;

	using TextureUnits = std::unordered_map<TextureUnit, std::vector<std::pair<TextureType, uint32_t>>>;
	TextureUnits _prevTextureUnits;

	std::vector<uint32_t> _prevShaders;

	using ProgramID = uint32_t;
	using UniformName = std::string;
	using UniformLocations = std::unordered_map<UniformName, uint32_t>;
	using ShaderUniformLocations = std::unordered_map<ProgramID, UniformLocations>;

	ShaderUniformLocations _UniformLocations;

	std::vector<std::pair<glm::vec2, glm::vec2>> _prevViewports;

	uint32_t FindUniformLocation(std::string name);

public:

	void PushViewport(glm::uvec2 pos, glm::uvec2 size) override;
	void PopViewport() override;

	void Clear(ClearFlag) override;
	void SetClearColor(std::array<float, 4> const) override;

	void DrawIndexed(SharedPtr<VertexArray> const &) override;

	void PushFramebuffer(SharedPtr<Framebuffer> const &) override;
	void PopFramebuffer() override;

	void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const override;
	void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src, CopyTarget) const override;
	void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> const &, CopyTarget) const override;

	void PushCapability(RendererCaps, bool enable) override;
	void PopCapability(RendererCaps) override;

	void PushTexture(SharedPtr<Texture> const &, TextureUnit) override;
	void PopTexture(TextureUnit) override;

	void PushShader(SharedPtr<Shader> const &) override;
	void PopShader() override;

	void SetDepthMask(bool val) override;

	void SetUniform(std::string const &name, size_t value) override;
	void SetUniform(std::string const &name, int32_t value) override;
	void SetUniform(std::string const &name, uint32_t value) override;
	void SetUniform(std::string const &name, float value) override;
	void SetUniform(std::string const &name, glm::vec3 value) override;
	void SetUniform(std::string const &name, glm::vec4 value) override;
	void SetUniform(std::string const &name, glm::mat3 value) override;
	void SetUniform(std::string const &name, glm::mat4 value) override;
	void SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size = std::nullopt) override;
	void SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size = std::nullopt) override;
};

}
