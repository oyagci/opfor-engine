#pragma once

#include "Renderer.hpp"
#include "opfor/core/base.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/GBuffer.hpp"
#include "opfor/renderer/Mesh.hpp"
#include "opfor/renderer/PerspectiveCamera.hpp"
#include "opfor/renderer/Shader.hpp"

struct PlayerCameraComponent;
struct TransformComponent;

namespace opfor
{

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

    static constexpr unsigned int ShadowWidth = 2048;
    static constexpr unsigned int ShadowHeight = 2048;

    void InitDepthCubemap();
    void InitQuad();
    void InitBillboard();
    Vector<DrawCommand> RenderShadowMeshes();
    Vector<DrawCommand> SubmitMeshes(PerspectiveCamera const &);
    Vector<UniformBinding> UpdateLight();
    Vector<DrawCommand> RenderLightBillboard(PerspectiveCamera const &);
    Vector<DrawCommand> RenderLight(PerspectiveCamera const &);
    RenderCommandBuffer RenderShadowMap();

  public:
    MeshRenderer();

    void RenderMeshes(PerspectiveCamera const &);
    void Resize(unsigned int width, unsigned int height);
};

} // namespace opfor
