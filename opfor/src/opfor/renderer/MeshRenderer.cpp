#include "MeshRenderer.hpp"

#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include "opfor/core/Application.hpp"
#include "opfor/core/ImageLoader.hpp"
#include "opfor/renderer/Renderer.hpp"
#include "opfor/renderer/ShaderManager.hpp"
#include "opfor/renderer/TextureManager.hpp"

#include "components/DirectionalLightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "components/TransformComponent.hpp"

#include <glm/gtc/quaternion.hpp>

namespace opfor
{

MeshRenderer::MeshRenderer()
{
    InitQuad();
    InitBillboard();
    InitDepthCubemap();

    auto texture = TextureManager::Get().Create2D("light_bulb_icon");
    auto img = ImageLoader::Load("./img/light_bulb_icon.png");

    TextureParameterList texParams = {
        {TextureParameterType::MignifyFilter, TextureParameterValue::LinearMipmapLinear},
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
    };

    texture->SetDataType(DataType::UnsignedByte);
    texture->SetHasAlpha(img.nchannel == 4);
    texture->SetIsSRGB(true);
    texture->SetInputFormat(img.nchannel == 4 ? DataFormat::RGBA : DataFormat::RGB);
    texture->SetOutputFormat(img.nchannel == 4 ? DataFormat::RGBA : DataFormat::RGB);
    texture->SetSize(img.width, img.height);
    texture->SetTextureParameters(texParams);
    texture->SetGenerateMipmap(true);
    texture->SetData(img.data.get());
    texture->Build();

    _light = Shader::Create("shaders/light.glsl");

    _light->Bind();
    _light->SetUniform("gPosition", 0);
    _light->SetUniform("gNormal", 1);
    _light->SetUniform("gAlbedoSpec", 2);
    _light->SetUniform("gSSAO", 3);
    _light->SetUniform("depthMap", 4);
    _light->SetUniform("gMetallicRoughness", 5);
    _light->SetUniform("irradianceMap", 6);
    _light->SetUniform("brdfLUT", 7);
    _light->SetUniform("prefilterMap", 8);
    _light->Unbind();
}

void MeshRenderer::InitDepthCubemap()
{
    _shadow = Shader::Create("shaders/shadow.glsl");

    float aspect = static_cast<float>(ShadowWidth) / static_cast<float>(ShadowHeight);
    float n = 1.0f;
    float f = 1000.0f;
    _shadowProjection = glm::perspective(glm::radians(90.0f), aspect, n, f);

    _depthCubemap = TextureCubemap::Create();

    _depthCubemap->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::MignifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
    });
    _depthCubemap->SetInputFormat(DataFormat::Depth);
    _depthCubemap->SetOutputFormat(DataFormat::Depth);
    _depthCubemap->SetDataType(DataType::Float);
    _depthCubemap->SetSize(ShadowWidth, ShadowHeight);

    _depthCubemap->Bind(TextureUnit::Texture0);

    constexpr std::array<CubemapFace, 6> faces{
        CubemapFace::PositiveX, CubemapFace::NegativeX, CubemapFace::PositiveY,
        CubemapFace::NegativeY, CubemapFace::PositiveZ, CubemapFace::NegativeZ,
    };

    _depthCubemap->Bind(TextureUnit::Texture0);

    for (size_t i = 0; i < faces.size(); i++)
    {
        ImageLoader::Image img{};

        img.width = ShadowWidth;
        img.height = ShadowHeight;

        _depthCubemap->SetFaceData(faces[i], std::move(img));
    }

    _depthCubemap->Build();

    _depthmap = Framebuffer::Create();
    _depthmap->Bind();
    _depthmap->AttachTexture(_depthCubemap, FramebufferAttachment::DepthAttachment);
    _depthmap->Unbind();
}

void MeshRenderer::InitQuad()
{
    std::array<glm::vec3, 4> pos = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(-1.0f, 1.0f, 0.0f),
    };
    std::array<glm::vec2, 4> tex = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
    };

    for (auto p : pos)
    {
        _quad.addPosition(p);
    }
    for (auto t : tex)
    {
        _quad.addUv(t);
    }
    _quad.addTriangle(glm::uvec3(0, 1, 2));
    _quad.addTriangle(glm::uvec3(0, 2, 3));
    _quad.build();
}

void MeshRenderer::InitBillboard()
{
    _billboard = Shader::Create("shaders/billboard.glsl");
}

Vector<DrawCommand> MeshRenderer::RenderShadowMeshes()
{
    Vector<DrawCommand> drawCommands;

    auto entities = Application::Get().GetEntities<ModelComponent, TransformComponent>();

    for (auto const entity : entities)
    {

        auto [model, transform] = entity->GetAll();

        for (auto const meshId : model.meshes)
        {

            auto const *mesh = Application::Get().GetMesh(meshId);

            Mat4 modelMatrix(CalcModelMatrix(transform));

            DrawCommand cmd;
            cmd.uniformBindings = {{"modelMatrix", modelMatrix}};
            cmd.vertexArray = reinterpret_cast<Mesh const *>(mesh)->GetVertexArray();
            drawCommands.push_back(cmd);
        }
    }

    return drawCommands;
}

Mat4 MeshRenderer::CalcModelMatrix(TransformComponent const &transform)
{
    Mat4 finalTransformation(1.0f);

    TransformComponent const *cur = &transform;

    while (cur->parent)
    {
        TransformComponent const &parent = cur->parent.value().get();

        auto model = glm::translate(Mat4(1.0f), parent.position) *
                                           glm::mat4_cast(parent.rotation) *
                                           glm::scale(Mat4(1.0f), parent.scale);

        finalTransformation = model * finalTransformation;

        cur = &cur->parent.value().get();
    }

    finalTransformation = finalTransformation * glm::translate(Mat4(1.0f), transform.position) *
                          glm::mat4_cast(transform.rotation) * glm::scale(Mat4(1.0f), transform.scale);

    return finalTransformation;
}

Vector<DrawCommand> MeshRenderer::SubmitMeshes(PerspectiveCamera const &camera)
{
    Vector<DrawCommand> drawCommands;

    auto models = Application::Get().GetEntities<ModelComponent, TransformComponent>();

    for (auto const &entity : models)
    {

        auto const [model, transform] = entity->GetAll();

        for (auto const meshId : model.meshes)
        {

            auto const mesh = Application::Get().GetMesh(meshId);
            auto const shaderId = model.shader;

            auto shaderOpt = ShaderManager::Get().Get(shaderId);

            if (!shaderOpt.has_value())
            {
                Logger::Warn("Shader {} does not exist\n", shaderId);
                continue;
            }

            auto &shader = shaderOpt.value();

            Mat4 modelMatrix(CalcModelMatrix(transform));

            DrawCommand drawCommand;
            drawCommand.shader = shader;
            drawCommand.uniformBindings = {
                {"viewProjectionMatrix", camera.GetViewProjectionMatrix()},
                {"viewMatrix", camera.GetViewMatrix()},
                {"projectionMatrix", camera.GetProjection()},
                {"viewPos", camera.GetPosition()},
                {"modelMatrix", modelMatrix},
            };

            // Bind each texture
            auto meshCast = dynamic_cast<Mesh *>(mesh);
            if (meshCast != nullptr)
            {
                if (meshCast->GetPbrMaterial().has_value())
                {

                    auto material = Application::Get().GetPbrMaterial(meshCast->GetPbrMaterial().value());

                    if (material.has_value())
                    {
                        auto m = material.value();

                        drawCommand.uniformBindings.push_back({"material.baseColor", m->BaseColor});
                        drawCommand.uniformBindings.push_back({"material.metallicFactor", m->MetallicFactor});
                        drawCommand.uniformBindings.push_back({"material.roughnessFactor", m->RoughnessFactor});

                        if (m->Albedo.has_value())
                        {
                            auto albedo = m->Albedo.value();
                            auto texture = TextureManager::Get().Get(albedo);

                            drawCommand.uniformBindings.push_back({"material.hasAlbedo", 1});
                            drawCommand.textureBindings.push_back({texture, TextureUnit::Texture0});
                        }
                        else
                        {
                            drawCommand.uniformBindings.push_back({"material.hasAlbedo", 0});
                        }

                        if (m->MetallicRoughness.has_value())
                        {
                            auto metallicRoughness = m->MetallicRoughness.value();
                            auto texture = TextureManager::Get().Get(metallicRoughness);

                            drawCommand.uniformBindings.push_back({"material.hasMetallicRoughness", 1});
                            drawCommand.textureBindings.push_back({texture, TextureUnit::Texture1});
                        }
                        else
                        {
                            drawCommand.uniformBindings.push_back({"material.hasMetallicRoughness", 0});
                        }

                        if (m->Normal.has_value())
                        {
                            auto normal = m->Normal.value();
                            auto texture = TextureManager::Get().Get(normal);

                            drawCommand.textureBindings.push_back({texture, TextureUnit::Texture2});
                        }
                        else
                        {
                            auto const defaultNormal = TextureManager::Get().Get("default_normal");

                            drawCommand.textureBindings.push_back({defaultNormal, TextureUnit::Texture2});
                        }
                    }
                }

                drawCommand.vertexArray = reinterpret_cast<Mesh const *>(mesh)->GetVertexArray();
                drawCommands.push_back(drawCommand);
            }
        }
    }

    return drawCommands;
}

Vector<UniformBinding> MeshRenderer::UpdateLight()
{
    Vector<UniformBinding> lightBindings;

    auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

    if (lights.size() == 0)
    {
        return lightBindings;
    }

    lightBindings.push_back({"pointLightCount", static_cast<int>(lights.size())});

    for (size_t i = 0; i < lights.size(); i++)
    {
        auto [light, transform] = lights[i]->GetAll();

        std::string pointLight = "pointLight[" + std::to_string(i) + "]";

        lightBindings.push_back({pointLight + ".position", transform.position});
        lightBindings.push_back({pointLight + ".color", light.Color});
        lightBindings.push_back({pointLight + ".intensity", light.Intensity});
    }

    auto dirLights = Application::Get().GetEntities<DirectionalLightComponent>();

    lightBindings.push_back({"directionalLightCount", static_cast<int>(dirLights.size())});

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        auto [light] = dirLights[i]->GetAll();

        std::string directionalLight = "directionalLights[" + std::to_string(i) + "]";

        lightBindings.push_back({directionalLight + ".direction", light.Direction});
        lightBindings.push_back({directionalLight + ".color", light.Color});
        lightBindings.push_back({directionalLight + ".intensity", light.Intensity});
    }

    return lightBindings;
}

Vector<DrawCommand> MeshRenderer::RenderLightBillboard(PerspectiveCamera const &camera)
{
    Vector<DrawCommand> drawCommands;

    auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

    if (lights.size() == 0)
    {
        return drawCommands;
    }

    for (auto const &lightEnt : lights)
    {
        auto [light, transform] = lightEnt->GetAll();

        DrawCommand drawCommand;
        drawCommand.shader = _billboard;
        drawCommand.uniformBindings = {
            {"viewMatrix", camera.GetViewMatrix()},
            {"viewProjectionMatrix", camera.GetViewProjectionMatrix()},
            {"projectionMatrix", camera.GetProjection()},
            {"particlePosition", transform.position},
        };
        drawCommand.textureBindings = {{TextureManager::Get().Get("light_bulb_icon"), TextureUnit::Texture0}};
        drawCommand.vertexArray = _quad.GetVertexArray();
        drawCommands.push_back(drawCommand);
    }

    return drawCommands;
}

Vector<DrawCommand> MeshRenderer::RenderLight(PerspectiveCamera const &camera)
{
    DrawCommand drawCommand;
    drawCommand.shader = _light;
    drawCommand.uniformBindings = {
        {"viewPos", camera.GetPosition()},
        {"exposure", camera.GetExposure()},
    };
    drawCommand.textureBindings = {
        {_gBuffer.GetPositionTex(), TextureUnit::Texture0},
        {_gBuffer.GetNormalTex(), TextureUnit::Texture1},
        {_gBuffer.GetAlbedoSpecTex(), TextureUnit::Texture2},
        {_depthCubemap, TextureUnit::Texture4},
        {_gBuffer.GetMetallicRoughnessTex(), TextureUnit::Texture5},
        {TextureManager::Get().Get("irradianceMap"), TextureUnit::Texture6},
        {TextureManager::Get().Get("brdfLUT"), TextureUnit::Texture7},
        {TextureManager::Get().Get("prefilterMap"), TextureUnit::Texture8},
    };

    auto lightBindings = UpdateLight();
    drawCommand.uniformBindings.insert(drawCommand.uniformBindings.end(), lightBindings.begin(), lightBindings.end());

    drawCommand.vertexArray = {_quad.GetVertexArray()};

    return {drawCommand};
}

RenderCommandBuffer MeshRenderer::RenderShadowMap()
{
    RenderCommandBuffer renderCommand;

    auto lights = Application::Get().GetEntities<PointLightComponent, TransformComponent>();

    if (lights.size() == 0)
    {
        return renderCommand;
    };

    auto lightPos = lights[0]->Get<TransformComponent>().position;

    std::vector<Mat4> shadowTransforms = {
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
        _shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)),
    };

    renderCommand.framebuffer = _depthmap;
    renderCommand.viewportExtent = {{0, 0}, {ShadowWidth, ShadowHeight}};
    renderCommand.capabilities = {{RendererCaps::Blend, false}, {RendererCaps::DepthTest, true}};
    renderCommand.clear = {{0.0f, 0.0f, 0.0f, 1.0f}, ClearFlag::DepthBit | ClearFlag::ColorBit};

    Vector<DrawCommand> shadowDrawCommands = RenderShadowMeshes();
    for (auto &cmd : shadowDrawCommands)
    {
        cmd.shader = _shadow;
        cmd.uniformBindings.push_back({"model", Mat4(1.0f)});
        cmd.uniformBindings.push_back({"shadowMatrices", shadowTransforms});
        cmd.uniformBindings.push_back({"far_plane", 10000.0f});
        cmd.uniformBindings.push_back({"lightPos", lightPos});
    }

    renderCommand.drawCommands = shadowDrawCommands;

    return renderCommand;
}

void MeshRenderer::RenderMeshes(PerspectiveCamera const &camera)
{
    RenderCommandBuffer shadowMapRenderCommand = RenderShadowMap();
    Renderer::SubmitRenderCommandBuffer(shadowMapRenderCommand);

    RenderCommandBuffer renderCommand;
    renderCommand.framebuffer = _gBuffer.GetFramebuffer();
    renderCommand.clear = {{0.0f, 0.0f, 0.0f, 1.0f}, ClearFlag::ColorBit | ClearFlag::DepthBit};
    renderCommand.capabilities = {{RendererCaps::DepthTest, true}};
    renderCommand.drawCommands = SubmitMeshes(camera);
    Renderer::SubmitRenderCommandBuffer(renderCommand);

    RenderCommandBuffer lightRenderCommand;
    lightRenderCommand.clear = {{1.0f, 0.0f, 1.0f, 1.0f}, ClearFlag::ColorBit | ClearFlag::DepthBit};
    lightRenderCommand.drawCommands = RenderLight(camera);
    Renderer::SubmitRenderCommandBuffer(lightRenderCommand);

    // Copy depth buffer to viewport framebuffer to enable depth testing with billboard
    // and other shaders
    Renderer::CopyFramebufferToDefaultFramebuffer(_gBuffer.GetFramebuffer(), CopyTarget::DepthBufferBit);

    RenderCommandBuffer billboardRenderCommand;
    billboardRenderCommand.capabilities = {{RendererCaps::Blend, true}, {RendererCaps::DepthTest, true}};
    billboardRenderCommand.drawCommands = RenderLightBillboard(camera);
    Renderer::SubmitRenderCommandBuffer(billboardRenderCommand);
}

void MeshRenderer::Resize(unsigned int width, unsigned int height)
{
    _gBuffer.Resize(width, height);
}

} // namespace opfor
