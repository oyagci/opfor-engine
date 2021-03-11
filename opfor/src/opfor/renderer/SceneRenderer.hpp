#pragma once
#include "MeshRenderer.hpp"
#include "SkyboxRenderer.hpp"
#include "opfor/layers/Layer.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"

using namespace std::placeholders;

namespace opfor
{

class SceneRenderer : public Layer
{
  private:
    MeshRenderer _MeshRenderer;
    SkyboxRenderer _SkyboxRenderer;

  public:
    SceneRenderer() = default;
    ~SceneRenderer() = default;

    void RenderScene(PerspectiveCamera const &);

    void OnEvent(Event &e);
};

} // namespace opfor
