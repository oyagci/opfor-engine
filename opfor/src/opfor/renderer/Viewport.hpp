#pragma once

#include "opfor/core/events/Event.h"
#include "opfor/core/events/EngineEvents.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/Texture.hpp"
#include "opfor/layers/Layer.hpp"

#include <glm/vec2.hpp>

namespace opfor {

	class Viewport : public Layer
	{
	private:
		SharedPtr<Framebuffer> _Framebuffer;
		SharedPtr<Texture2D> _Texture;

		glm::uvec2 _Size;

	private:
		bool OnViewportResized(ViewportResizeEvent &e);

	public:
		Viewport(unsigned int width, unsigned int height);
		Viewport &operator=(Viewport const &) = delete;
		Viewport(Viewport const &) = delete;

		void OnEvent(Event &) override;

		uint32_t GetRawHandle() { return _Framebuffer->GetRawHandle(); }
		auto GetFramebuffer() { return _Framebuffer; }
		auto &GetSize() { return _Size; }
	};

}