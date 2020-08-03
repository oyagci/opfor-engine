#include "SceneRenderer.hpp"
#include "Application.hpp"

namespace opfor {

void SceneRenderer::RenderScene()
{
	_MeshRenderer.RenderMeshes();
	_SkyboxRenderer.RenderSkybox();
	opfor::Renderer::CopyDefaultFramebufferTo(opfor::Application::Get().GetViewport(), opfor::CopyTarget::ColorBufferBit);
}

}
