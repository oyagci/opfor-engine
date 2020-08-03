#pragma once
#include "SkyboxRenderer.hpp"
#include "MeshRenderer.hpp"

using namespace std::placeholders;

namespace opfor {

class SceneRenderer
{
private:
	MeshRenderer _MeshRenderer;
	SkyboxRenderer _SkyboxRenderer;

public:
	SceneRenderer() = default;
	~SceneRenderer() = default;

	void RenderScene();
};

}
