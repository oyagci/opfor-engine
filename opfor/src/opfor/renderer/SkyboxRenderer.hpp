#pragma once

#include "opfor/core/base.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/Mesh.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"
#include "opfor/renderer/Renderbuffer.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/renderer/Shader.hpp"
#include "opfor/renderer/Texture.hpp"

namespace opfor
{

class SkyboxRenderer
{
  private:
    opfor::SharedPtr<opfor::Shader> _shader;
    opfor::SharedPtr<opfor::TextureCubemap> _SkyboxCubemap;
    opfor::SharedPtr<opfor::Shader> _SphericalToCubemap;
    opfor::Mesh _SkyboxCubeMesh;
    opfor::SharedPtr<opfor::TextureCubemap> _HDRI;
    opfor::SharedPtr<opfor::TextureCubemap> _Irradiance;
    opfor::SharedPtr<opfor::TextureCubemap> _Prefilter;
    opfor::SharedPtr<opfor::Texture2D> _BRDF;
    opfor::Mesh _Quad;

    void InitQuad();
    opfor::SharedPtr<opfor::Texture2D> LoadHDRI();
    void InitHDRI();
    void InitSkybox();

  public:
    SkyboxRenderer();
    ~SkyboxRenderer() = default;

    void RenderSkybox(PerspectiveCamera const &camera);
};

} // namespace opfor
