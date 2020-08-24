#pragma once

#include "opfor/core/base.hpp"
#include "opfor/renderer/Shader.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/GBuffer.hpp"
#include "opfor/renderer/Mesh.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"

struct PlayerCameraComponent;
struct TransformComponent;

namespace opfor {

class MeshRenderer
{
private:
	SharedPtr<Shader> _billboard;
	SharedPtr<Shader> _light;
	SharedPtr<Shader> _shadow;
	Mesh _quad;

	GBuffer _gBuffer;

	SharedPtr<Framebuffer> _depthmap;
	SharedPtr<TextureCubemap> _depthCubemap;
	glm::mat4 _shadowProjection;

	static constexpr unsigned int ShadowWidth  = 2048;
	static constexpr unsigned int ShadowHeight = 2048;

	void InitDepthCubemap();
	void InitQuad();
	void InitBillboard();
	void RenderShadowMeshes();
	void RenderMeshes(PerspectiveCamera const &);
	void UpdateLight();
	void RenderLightBillboard(PerspectiveCamera const &);
	void RenderLight(PerspectiveCamera const &);
	void BakeShadowMap();

public:
	MeshRenderer();

	void RenderMeshes();
	void Resize(unsigned int width, unsigned int height);
};

}