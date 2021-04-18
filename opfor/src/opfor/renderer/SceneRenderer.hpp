#pragma once
#include "MeshRenderer.hpp"
#include "SkyboxRenderer.hpp"
#include "opfor/layers/Layer.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"

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

    void UpdateScene(float dt);
    void RenderScene(PerspectiveCamera const &);

    void OnEvent(Event &e);
};

} // namespace opfor
