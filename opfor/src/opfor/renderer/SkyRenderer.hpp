#pragma once

#include "opfor/core/Base.hpp"
#include "Shader.hpp"
#include "Quad.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"

namespace opfor {

	class SkyRenderer
	{
	private:
		SharedPtr<Shader> _Shader;
		SharedPtr<Framebuffer> _Framebuffer;
		SharedPtr<Texture2D> _Texture;
		SharedPtr<Shader> _QuadShader;
		Quad _Quad;

	public:
		SkyRenderer();
		SkyRenderer(SkyRenderer const &) = delete;
		SkyRenderer &operator=(SkyRenderer const &) = delete;

		void Render();
		void Resize(unsigned int width, unsigned int height);
	};

}