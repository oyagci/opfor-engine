#pragma once

#include "opfor/core/base.hpp"
#include "opfor/renderer/Shader.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "GBuffer.hpp"
#include "Mesh.hpp"

struct PlayerCameraComponent;
struct TransformComponent;

namespace opfor {

class MeshRenderer
{
private:
	opfor::SharedPtr<opfor::Shader> _billboard;
	opfor::SharedPtr<opfor::Shader> _light;
	opfor::SharedPtr<opfor::Shader> _shadow;
	opfor::Mesh _quad;

	GBuffer _gBuffer;

	opfor::SharedPtr<opfor::Framebuffer> _depthmap;
	opfor::SharedPtr<opfor::TextureCubemap> _depthCubemap;
	glm::mat4 _shadowProjection;

	static constexpr unsigned int ShadowWidth  = 2048;
	static constexpr unsigned int ShadowHeight = 2048;

	void InitDepthCubemap();
	void InitQuad();
	void InitBillboard();
	void RenderShadowMeshes();
	void RenderMeshes(PlayerCameraComponent const &camera, TransformComponent const &playerTransform);
	void UpdateLight();
	void RenderLightBillboard(PlayerCameraComponent const &camera);
	void RenderLight(glm::vec3 const &viewPos, float const exposure);
	void BakeShadowMap();

public:
	MeshRenderer();

	void RenderMeshes();
};

}

	// GLuint _ssaoBlurFb;
	// GLuint _ssaoNoiseTex;
	// GLuint _ssaoBlurTex;
	// opfor::SharedPtr<opfor::Shader> _ssaoShader;
	// opfor::SharedPtr<opfor::Shader> _ssaoBlurShader;
	// opfor::SharedPtr<opfor::Framebuffer> _ssao;
	// GLuint _ssaoColorBuf;

	//void InitSSAO()
	//{
	//	auto display = opfor::Application::Get().GetWindow();
	//	auto [ width, height ] = std::tuple(display->GetWidth(), display->GetHeight());

	//	_ssaoShader = opfor::Shader::Create();
	//	_ssaoShader->AddVertexShader("shaders/ssao.vs.glsl");
	//	_ssaoShader->AddFragmentShader("shaders/ssao.fs.glsl");
	//	_ssaoShader->Link();

	//	glGenFramebuffers(1, &_ssaoFb);
	//	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFb);

	//	glGenTextures(1, &_ssaoColorBuf);
	//	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuf, 0);
	//	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	GenSSAOKernel();

	//	_ssaoShader->Bind();
	//	_ssaoShader->SetUniform("gPosition", 0);
	//	_ssaoShader->SetUniform("gNormal", 1);
	//	_ssaoShader->SetUniform("texNoise", 2);
	//	_ssaoShader->Unbind();

	//	_ssaoBlurShader = opfor::Shader::Create();
	//	_ssaoBlurShader->AddVertexShader("shaders/ssao.vs.glsl");
	//	_ssaoBlurShader->AddFragmentShader("shaders/ssaoblur.fs.glsl");
	//	_ssaoBlurShader->Link();

	//	_ssaoBlurShader->Bind();
	//	_ssaoBlurShader->SetUniform("ssaoInput", 0);
	//	_ssaoBlurShader->Unbind();

	//	glGenFramebuffers(1, &_ssaoBlurFb);
	//	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFb);

	//	glGenTextures(1, &_ssaoBlurFb);
	//	glBindTexture(GL_TEXTURE_2D, _ssaoBlurFb);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuf, 0);
	//}

	//void GenSSAOKernel()
	//{
	//	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	//	std::default_random_engine generator;
	//	std::vector<glm::vec3> ssaoKernel;

	//	auto lerp = [] (float a, float b, float f) -> float {
	//		return a + f * (b - a);
	//	};

	//	for (size_t i = 0; i < 64; i++) {
	//		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f,
	//						 randomFloats(generator) * 2.0f - 1.0f,
	//						 randomFloats(generator));
	//		sample = glm::normalize(sample);
	//		sample *= randomFloats(generator);

	//		// Make distribution closer to origin
	//		float scale = static_cast<float>(i) / 64.0f;
	//		scale = lerp(0.1f, 1.0f, scale * scale);
	//		sample *= scale;

	//		ssaoKernel.push_back(sample);
	//	}

	//	std::array<glm::vec3, 64> ssaoNoise;
	//	for (size_t i = 0; i < 64; i++) {
	//		glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f,
	//						randomFloats(generator) * 2.0f - 1.0f,
	//						0.0f);
	//		ssaoNoise[i] = noise;
	//	}

	//	glGenTextures(1, &_ssaoNoiseTex);
	//	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glBindTexture(GL_TEXTURE_2D, 0);

	//	_ssaoShader->Bind();
	//	_ssaoShader->SetUniform("samples", ssaoKernel, 64);
	//	_ssaoShader->Unbind();
	//}

	//void RenderSSAO(PlayerCameraComponent const &camera)
	//{
	//	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFb);
	//	_ssaoShader->Bind();
	//	_ssaoShader->SetUniform("gPosition", 0);
	//	_ssaoShader->SetUniform("gNormal", 1);
	//	_ssaoShader->SetUniform("texNoise", 2);

	//	glClear(GL_COLOR_BUFFER_BIT);
	//	// glActiveTexture(GL_TEXTURE0);
	//		// glBindTexture(GL_TEXTURE_2D, _gBuffer.GetPositionTex());
	//	// glActiveTexture(GL_TEXTURE1);
	//	// 	glBindTexture(GL_TEXTURE_2D, _gBuffer.GetNormalTex());
	//	glActiveTexture(GL_TEXTURE2);
	//		glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);

	//	_ssaoShader->SetUniform("projectionMatrix", camera.projection);
	//	_ssaoShader->SetUniform("viewMatrix", camera.view);

	//	opfor::Renderer::Submit(_quad.GetVertexArray());

	//	glActiveTexture(GL_TEXTURE0);
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//	glActiveTexture(GL_TEXTURE1);
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//	glActiveTexture(GL_TEXTURE2);
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//	_ssaoShader->Unbind();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	//	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoBlurFb);
//
//	//	_ssaoBlurShader.bind();
//	//	_ssaoBlurShader.setUniform1i("ssaoInput", 0);
//	//	glActiveTexture(GL_TEXTURE0);
//	//	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuf);
//	//	_quad.Draw();
//	//	glBindTexture(GL_TEXTURE_2D, 0);
//	//	_ssaoShader.unbind();

//	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}
