#include "SceneRenderer.hpp"
#include "opfor/core/Application.hpp"

namespace opfor {

void SceneRenderer::RenderScene()
{
	_MeshRenderer.RenderMeshes();
	_SkyboxRenderer.RenderSkybox();
	_SkyRenderer.Render();
	opfor::Renderer::CopyDefaultFramebufferTo(opfor::Application::Get().GetViewport()->GetFramebuffer(), opfor::CopyTarget::ColorBufferBit);
}

void SceneRenderer::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);

	dispatcher.DispatchIf<ViewportResizeEvent>([&](ViewportResizeEvent &e) -> bool {
		_MeshRenderer.Resize(e.GetWidth(), e.GetHeight());
		_SkyRenderer.Resize(e.GetWidth(), e.GetHeight());
		return false;
	});
}

}
