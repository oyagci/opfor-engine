#include "SkyboxRenderer.hpp"
#include "opfor/core/Application.hpp"
#include "opfor/renderer/TextureManager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>

#include "components/SkyboxComponent.hpp"

namespace opfor
{

SkyboxRenderer::SkyboxRenderer()
{
    _shader = Shader::Create("shaders/skybox.glsl");
    _SphericalToCubemap = Shader::Create("shaders/hdri-cubemap-projection.glsl");

    InitQuad();
    InitSkybox();
    InitHDRI();
}

void SkyboxRenderer::InitQuad()
{
    Array<Vec3, 4> pos = {
        Vec3(-1.0f, -1.0f, 0.0f),
        Vec3(1.0f, -1.0f, 0.0f),
        Vec3(1.0f, 1.0f, 0.0f),
        Vec3(-1.0f, 1.0f, 0.0f),
    };
    Array<Vec2, 4> tex = {
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f),
    };

    for (auto p : pos)
    {
        _Quad.addPosition(p);
    }
    for (auto t : tex)
    {
        _Quad.addUv(t);
    }
    _Quad.addTriangle(UVec3(0, 1, 2));
    _Quad.addTriangle(UVec3(0, 2, 3));
    _Quad.build();
}

SharedPtr<Texture2D> SkyboxRenderer::LoadHDRI()
{
    stbi_set_flip_vertically_on_load(1);
    auto image = opfor::ImageLoader::LoadHDR("assets/hdri/ballroom_4k.hdr");
    stbi_set_flip_vertically_on_load(0);

    auto texture = Texture2D::Create();
    texture->SetData(image.data.get());
    texture->SetDataType(DataType::Float);
    texture->SetInputFormat(DataFormat::RGB16F);
    texture->SetOutputFormat(DataFormat::RGB);
    texture->SetSize(image.width, image.height);
    texture->SetIsSRGB(true);
    texture->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::MignifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
    });

    texture->Build();

    return texture;
}

void SkyboxRenderer::InitHDRI()
{
    SharedPtr<Texture2D> hdrTexture = LoadHDRI();
    SharedPtr<Framebuffer> capture = Framebuffer::Create();
    SharedPtr<Renderbuffer> captureRbo = Renderbuffer::Create();

    captureRbo->AttachDepthComponent({2048, 2048});
    capture->AttachRenderbuffer(captureRbo, RenderbufferAttachment::DepthAttachment);

    SharedPtr<TextureCubemap> envCubemap = TextureCubemap::Create();
    envCubemap->SetInputFormat(DataFormat::RGB16F);
    envCubemap->SetOutputFormat(DataFormat::RGB);
    envCubemap->SetSize(2048, 2048);
    envCubemap->SetDataType(DataType::Float);
    envCubemap->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::MignifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
    });

    ImageLoader::Image emptyImage{};
    emptyImage.width = 2048;
    emptyImage.height = 2048;

    envCubemap->Bind(TextureUnit::Texture0);
    for (size_t i = 0; i < 6; i++)
    {
        envCubemap->SetFaceData((CubemapFace)((GLint)CubemapFace::PositiveX + i), std::move(emptyImage));
    }

    envCubemap->Build();

    Mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));
    Array<Mat4, 6> views = {
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f))};

    _SphericalToCubemap->Bind();
    _SphericalToCubemap->SetUniform("equirectangularMap", 0);
    _SphericalToCubemap->SetUniform("projectionMatrix", projection);
    hdrTexture->Bind(TextureUnit::Texture0);
    RenderCommand::PushViewport({0, 0}, {2048, 2048});

    capture->Bind();

    for (size_t i = 0; i < 6; i++)
    {
        _SphericalToCubemap->SetUniform("viewMatrix", views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), envCubemap->GetRawHandle(), 0);
        RenderCommand::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);
        _SkyboxCubeMesh.Draw();
    }

    capture->Unbind();

    RenderCommand::PopViewport();
    _SphericalToCubemap->Unbind();

    _HDRI = envCubemap;

    ////////////////////////////////////////////////////////

    auto irradianceShader = Shader::Create("shaders/convolute.glsl");
    _Irradiance = TextureCubemap::Create();
    _Irradiance->SetInputFormat(DataFormat::RGB16F);
    _Irradiance->SetOutputFormat(DataFormat::RGB);
    _Irradiance->SetDataType(DataType::Float);
    _Irradiance->SetSize(32, 32);
    _Irradiance->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::MignifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
    });

    emptyImage.width = 32;
    emptyImage.height = 32;

    _Irradiance->Bind(TextureUnit::Texture0);
    for (size_t i = 0; i < 6; i++)
    {
        _Irradiance->SetFaceData((CubemapFace)((GLint)CubemapFace::PositiveX + i), std::move(emptyImage));
    }
    _Irradiance->Build();

    capture->Bind();
    captureRbo->Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradianceShader->Bind();
    irradianceShader->SetUniform("cubemap", 0);
    irradianceShader->SetUniform("projectionMatrix", projection);
    envCubemap->Bind(TextureUnit::Texture0);
    RenderCommand::PushViewport({0, 0}, {32, 32});

    for (size_t i = 0; i < 6; i++)
    {
        irradianceShader->SetUniform("viewMatrix", views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), _Irradiance->GetRawHandle(), 0);
        RenderCommand::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);
        _SkyboxCubeMesh.Draw();
    }
    capture->Unbind();
    RenderCommand::PopViewport();
    irradianceShader->Unbind();

    TextureManager::Get().Add("irradianceMap", _Irradiance);

    ///////////////////////////////////////////////////////

    SharedPtr<Shader> prefilterShader = Shader::Create("shaders/prefilter.glsl");

    emptyImage.width = 128;
    emptyImage.height = 128;

    _Prefilter = TextureCubemap::Create();
    _Prefilter->SetInputFormat(DataFormat::RGB16F);
    _Prefilter->SetOutputFormat(DataFormat::RGB);
    _Prefilter->SetDataType(DataType::Float);
    _Prefilter->SetSize(128, 128);
    _Prefilter->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::MignifyFilter, TextureParameterValue::LinearMipmapLinear},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
    });
    _Prefilter->SetGenerateMipmap(true);
    _Prefilter->Bind(TextureUnit::Texture0);
    for (size_t i = 0; i < 6; i++)
    {
        envCubemap->SetFaceData((CubemapFace)((GLint)CubemapFace::PositiveX + i), std::move(emptyImage));
    }
    _Prefilter->Build();

    RenderCommand::PushShader(prefilterShader);

    RenderCommand::SetUniform("cubemap", 0);
    RenderCommand::SetUniform("projectionMatrix", projection);

    RenderCommand::PushTexture(_HDRI, TextureUnit::Texture0);
    RenderCommand::PushFramebuffer(capture);

    constexpr size_t maxMipLevel = 5;
    for (size_t mip = 0; mip < maxMipLevel; mip++)
    {

        const unsigned int mipSize = 128 >> mip;

        captureRbo->Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);
        RenderCommand::PushViewport({0, 0}, {mipSize, mipSize});

        float roughness = (float)mip / (float)(maxMipLevel - 1);
        prefilterShader->SetUniform("roughness", roughness);

        for (unsigned int i = 0; i < 6; i++)
        {
            prefilterShader->SetUniform("viewMatrix", views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   _Prefilter->GetRawHandle(), static_cast<GLint>(mip));
            RenderCommand::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);
            _SkyboxCubeMesh.Draw();
        }
        RenderCommand::PopViewport();
    }

    RenderCommand::PopFramebuffer();
    RenderCommand::PopTexture(TextureUnit::Texture0);
    RenderCommand::PopShader();

    TextureManager::Get().Add("prefilterMap", _Prefilter);

    ////////////////////////////////////////////////////////////////////////

    SharedPtr<Shader> brdfShader = Shader::Create("shaders/brdf-precompute.glsl");

    _BRDF = Texture2D::Create();
    _BRDF->SetTextureParameters({
        {TextureParameterType::MignifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Linear},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
    });
    _BRDF->SetInputFormat(DataFormat::RGB16F);
    _BRDF->SetOutputFormat(DataFormat::RG);
    _BRDF->SetDataType(DataType::Float);
    _BRDF->SetSize(512, 512);
    _BRDF->Build();

    RenderCommand::PushFramebuffer(capture);

    captureRbo->Bind();
    // capture->AttachTexture(_BRDF, FramebufferAttachment::ColorAttachment0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _BRDF->GetRawHandle(), 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

    RenderCommand::PushViewport({0, 0}, {512, 512});
    RenderCommand::PushShader(brdfShader);

    RenderCommand::Clear(ClearFlag::ColorBit | ClearFlag::DepthBit);

    _Quad.Draw();

    RenderCommand::PopShader();
    RenderCommand::PopViewport();

    TextureManager::Get().Add("brdfLUT", _BRDF);
}

void SkyboxRenderer::InitSkybox()
{
    const Array<String, 6> paths = {
        "./img/right.jpg", "./img/left.jpg", "./img/top.jpg", "./img/bottom.jpg", "./img/front.jpg", "./img/back.jpg",
    };

    TextureParameterList params = {
        {TextureParameterType::MignifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapT, TextureParameterValue::ClampToEdge},
    };

    _SkyboxCubemap = TextureCubemap::Create();
    _SkyboxCubemap->SetTextureParameters(params);
    _SkyboxCubemap->SetInputFormat(DataFormat::RGB);
    _SkyboxCubemap->SetOutputFormat(DataFormat::RGB);
    _SkyboxCubemap->SetDataType(DataType::UnsignedByte);

    stbi_set_flip_vertically_on_load(false);

    constexpr Array<CubemapFace, 6> faces{
        CubemapFace::PositiveX, CubemapFace::NegativeX, CubemapFace::PositiveY,
        CubemapFace::NegativeY, CubemapFace::PositiveZ, CubemapFace::NegativeZ,
    };

    _SkyboxCubemap->Bind(TextureUnit::Texture0);

    for (size_t i = 0; i < faces.size(); i++)
    {
        ImageLoader::Image img = ImageLoader::Load(paths[i]);
        _SkyboxCubemap->SetFaceData(faces[i], std::move(img));
    }

    _SkyboxCubemap->Build();

    TextureManager::Get().Add("skybox-cubemap", _SkyboxCubemap);

    const Vec3 verts[] = {
        Vec3(-1.0, -1.0, 1.0),  Vec3(1.0, -1.0, 1.0),  Vec3(1.0, 1.0, 1.0),  Vec3(-1.0, 1.0, 1.0),

        Vec3(-1.0, -1.0, -1.0), Vec3(1.0, -1.0, -1.0), Vec3(1.0, 1.0, -1.0), Vec3(-1.0, 1.0, -1.0),
    };

    const UVec3 indices[] = {// front
                                  UVec3(0, 3, 1), UVec3(1, 3, 2),
                                  // right
                                  UVec3(1, 2, 5), UVec3(2, 6, 5),
                                  // back
                                  UVec3(4, 5, 6), UVec3(4, 6, 7),
                                  // left
                                  UVec3(0, 4, 7), UVec3(0, 7, 3),
                                  // bottom
                                  UVec3(0, 1, 4), UVec3(1, 5, 4),
                                  // top
                                  UVec3(3, 7, 2), UVec3(2, 7, 6)};

    _SkyboxCubeMesh.addTexture("skybox-cubemap", Mesh::TextureType::TT_Diffuse);
    for (const auto &v : verts)
    {
        _SkyboxCubeMesh.addPosition(v);
    }
    for (const auto &i : indices)
    {
        _SkyboxCubeMesh.addTriangle(i);
    }
    _SkyboxCubeMesh.build();
}

void SkyboxRenderer::RenderSkybox(PerspectiveCamera const &camera)
{
    auto skybox = Application::Get().GetEntities<SkyboxComponent>();

    if (skybox.size() == 0)
    {
        return;
    }

    DrawCommand drawCommand;
    drawCommand.shader = _shader;
    drawCommand.uniformBindings = {
        {"viewMatrix", Mat4(Mat3(camera.GetViewMatrix()))},
        {"projectionMatrix", camera.GetProjection()},
    };
    drawCommand.textureBindings = {{_HDRI, TextureUnit::Texture0}};
    drawCommand.vertexArray = _SkyboxCubeMesh.GetVertexArray();

    RenderCommandBuffer renderCommand;
    renderCommand.disableDepthMask = true;
    renderCommand.drawCommands = {drawCommand};

    Renderer::SubmitRenderCommandBuffer(renderCommand);
}

} // namespace opfor
