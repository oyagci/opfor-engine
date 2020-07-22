#pragma once

#include "engine/renderer/RendererAPI.hpp"

namespace opfor {

class OpenGLRendererAPI : public RendererAPI
{
private:
	std::vector<uint32_t> _prevReadBuffers;
	std::vector<uint32_t> _prevDrawBuffers;

	using CapState = std::unordered_map<RendererCaps, std::vector<bool>>;
	CapState _capStates;

	using TextureUnits = std::unordered_map<TextureUnit, std::vector<uint32_t>>;
	TextureUnits _prevTextureUnits;

public:
	void Clear() override;
	void SetClearColor(std::array<float, 4> const) override;

	void DrawIndexed(SharedPtr<VertexArray> const &) override;

	void PushFramebuffer(SharedPtr<Framebuffer> const &) override;
	void PopFramebuffer() override;

	void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const override;
	void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src, CopyTarget) const override;

	void PushCapability(RendererCaps, bool enable) override;
	void PopCapability(RendererCaps) override;

	void PushTexture(SharedPtr<Texture> const &, TextureUnit) override;
	void PopTexture(TextureUnit) override;

};

}
