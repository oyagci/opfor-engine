#pragma once

#include "opfor/core/base.hpp"
#include "opfor/renderer/Mesh.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"
#include "opfor/renderer/Shader.hpp"
#include "opfor/renderer/Texture.hpp"

namespace opfor
{

class SkyboxRenderer
{
  private:
    SharedPtr<Shader> _shader;
    SharedPtr<TextureCubemap> _SkyboxCubemap;
    SharedPtr<Shader> _SphericalToCubemap;
    Mesh _SkyboxCubeMesh;
    SharedPtr<TextureCubemap> _HDRI;
    SharedPtr<TextureCubemap> _Irradiance;
    SharedPtr<TextureCubemap> _Prefilter;
    SharedPtr<Texture2D> _BRDF;
    Mesh _Quad;

    void InitQuad();
    SharedPtr<Texture2D> LoadHDRI();
    void InitHDRI();
    void InitSkybox();

  public:
    SkyboxRenderer();
    ~SkyboxRenderer() = default;

    void RenderSkybox(PerspectiveCamera const &camera);
};

} // namespace opfor
