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
		inline static int       NumInScatteringPoints      = 10.0f;
		inline static int       NumOpticalScatteringPoints = 10.0f;
		inline static float     AtmosphereRadius           = 120.0f;
		inline static glm::vec3 PlanetCenter               = { 0.0f, 0.0f, 0.0f };
		inline static float     PlanetRadius               = 100.0f / 2.0f;
		inline static glm::vec3 SunPosition                = { 0.0f, 0.0f, -500.0f };
		inline static float     DensityFalloff             = 10.0f;
		inline static glm::vec3 Wavelengths                = { 700.f, 530.f, 440.f };
		inline static float     ScatteringStrength         = 1.0f;

	public:
		SkyRenderer();
		SkyRenderer(SkyRenderer const &) = delete;
		SkyRenderer &operator=(SkyRenderer const &) = delete;

		void Render();
		void Resize(unsigned int width, unsigned int height);
	};

}