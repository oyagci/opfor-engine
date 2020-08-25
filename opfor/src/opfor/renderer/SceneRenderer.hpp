#pragma once
#include "SkyboxRenderer.hpp"
#include "MeshRenderer.hpp"
#include "opfor/layers/Layer.hpp"
#include "SkyRenderer.hpp"

using namespace std::placeholders;

namespace opfor {

class SceneRenderer : public Layer
{
private:
	MeshRenderer _MeshRenderer;
	SkyboxRenderer _SkyboxRenderer;
	SkyRenderer _SkyRenderer;

public:
	SceneRenderer() = default;
	~SceneRenderer() = default;

	void RenderScene();

	void OnEvent(Event &e);
};

}
