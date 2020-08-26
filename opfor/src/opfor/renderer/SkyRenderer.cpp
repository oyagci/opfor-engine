#include "SkyRenderer.hpp"
#include "Renderer.hpp"
#include "opfor/core/Application.hpp"

namespace opfor {

	SkyRenderer::SkyRenderer()
	{
		_Shader = Shader::Create("shaders/sky.glsl");
		_QuadShader = Shader::Create("shaders/image.glsl");

		TextureParameterList texParams = {
			{ TextureParameterType::MignifyFilter, TextureParameterValue::Linear },
			{ TextureParameterType::MagnifyFilter, TextureParameterValue::Linear },
			{ TextureParameterType::WrapT,         TextureParameterValue::ClampToEdge },
			{ TextureParameterType::WrapS,         TextureParameterValue::ClampToEdge },
		};

		auto viewportSize = Application::Get().GetViewport()->GetSize();

		_Texture = Texture2D::Create();
			_Texture->SetTextureParameters(texParams);
			_Texture->SetInputFormat(DataFormat::RGBA16F);
			_Texture->SetOutputFormat(DataFormat::RGBA);
			_Texture->SetDataType(DataType::Float);
			_Texture->SetSize(viewportSize.x, viewportSize.y);
			_Texture->Build();

		_Framebuffer = Framebuffer::Create();
		_Framebuffer->AttachTexture(_Texture, FramebufferAttachment::ColorAttachment0);
	}

	void SkyRenderer::Render()
	{
		// Calculate Sky
		
		auto pos = Application::Get().GetCameraController().GetCamera().GetPosition();

		Renderer::DebugString(">>> Start Sky Calculations.");
		
		Renderer::Shader::Push(_Shader);
		
		auto &camera = Application::Get().GetCameraController().GetCamera();

		Renderer::Shader::SetUniform("viewPos", camera.GetPosition());
		Renderer::Shader::SetUniform("viewDir", glm::normalize(camera.GetDirection()));
		Renderer::Shader::SetUniform("inverseProjectionMatrix", glm::inverse(camera.GetProjection()));
		Renderer::Shader::SetUniform("inverseViewMatrix", glm::inverse(camera.GetViewMatrix()));

		Renderer::Shader::SetUniform("gColor",    1);
		Renderer::Shader::SetUniform("gDepth",    2);
		Renderer::Shader::SetUniform("gPosition", 3);

		Renderer::Shader::SetUniform("numInScatteringPoints",      NumInScatteringPoints);
		Renderer::Shader::SetUniform("numOpticalScatteringPoints", NumOpticalScatteringPoints);
		Renderer::Shader::SetUniform("atmosphereRadius",           AtmosphereRadius);
		Renderer::Shader::SetUniform("planetCenter",               PlanetCenter);
		Renderer::Shader::SetUniform("planetRadius",               PlanetRadius);
		Renderer::Shader::SetUniform("sunPosition",                SunPosition);
		Renderer::Shader::SetUniform("densityFalloff",             DensityFalloff);

		float scatterR = pow(400.0f / Wavelengths.x, 4.0) / ScatteringStrength;
		float scatterG = pow(400.0f / Wavelengths.y, 4.0) / ScatteringStrength;
		float scatterB = pow(400.0f / Wavelengths.z, 4.0) / ScatteringStrength;

		glm::vec3 scatteringCoefficients(scatterR, scatterG, scatterB);

		Renderer::Shader::SetUniform("scatteringCoefficients", scatteringCoefficients);

		Renderer::PushTexture(GBuffer::Get().GetAlbedoSpecTex(), TextureUnit::Texture1);
		Renderer::PushTexture(GBuffer::Get().GetDepthTex(),      TextureUnit::Texture2);
		Renderer::PushTexture(GBuffer::Get().GetPositionTex(),   TextureUnit::Texture3);

		// Renderer::Shader::SetUniform("viewMatrix", Application::Get().GetCameraController().GetCamera().GetViewMatrix());
		// Renderer::Shader::SetUniform("viewProjectionMatrix", Application::Get().GetCameraController().GetCamera().GetViewProjectionMatrix());
		// Renderer::Shader::SetUniform("projectionMatrix", Application::Get().GetCameraController().GetCamera().GetProjection());
		// Renderer::Shader::SetUniform("dimensions", glm::vec2(Application::Get().GetViewport()->GetSize()));
		
		Renderer::Submit([&]() {

			glBindImageTexture(0, _Texture->GetRawHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

			auto sz = Application::Get().GetViewport()->GetSize();
			glDispatchCompute(sz.x, sz.y, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		});

		Renderer::Shader::Pop();

		Renderer::DebugString("<<< Done Sky Calculations.");

		// Draw Sky

		Renderer::DebugString(">>> Start Sky Draw.");

		Renderer::PushCapability(RendererCaps::DepthTest, false);

		Renderer::Shader::Push(_QuadShader);
		Renderer::Shader::SetUniform("image", 0);
		Renderer::PushTexture(_Texture, TextureUnit::Texture0);

		Renderer::SetClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		Renderer::Clear(ClearFlag::ColorBit);
		
		_Quad.Draw();
		
		Renderer::PopTexture(TextureUnit::Texture0);
		Renderer::Shader::Pop();

		Renderer::PopCapability(RendererCaps::DepthTest);

		Renderer::DebugString("<<< End Sky Draw.");
	}

	void SkyRenderer::Resize(unsigned int width, unsigned int height)
	{
		_Texture->SetSize(width, height);
		_Texture->Build();
	}

}