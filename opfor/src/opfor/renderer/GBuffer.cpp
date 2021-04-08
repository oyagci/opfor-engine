#include "opfor/renderer/GBuffer.hpp"
#include "opfor/core/Application.hpp"

void GBuffer::Init()
{
    auto display = opfor::Application::Get().GetWindow();
    auto [width, height] = std::tuple(display->GetWidth(), display->GetHeight());

    _gBuffer = opfor::Framebuffer::Create();
    _gBuffer->Bind();

    using namespace opfor;

    TextureParameterList const params = {
        {TextureParameterType::MignifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::WrapR, TextureParameterValue::ClampToEdge},
        {TextureParameterType::WrapS, TextureParameterValue::ClampToEdge},
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest},
    };

    _gPosition = opfor::Texture2D::Create();
    _gPosition->SetTextureParameters(params);
    _gPosition->SetInputFormat(DataFormat::RGBA16F);
    _gPosition->SetOutputFormat(DataFormat::RGBA);
    _gPosition->SetDataType(DataType::Float);
    _gPosition->SetSize(width, height);
    _gPosition->Build();

    _gNormal = opfor::Texture2D::Create();
    _gNormal->SetTextureParameters(params);
    _gNormal->SetInputFormat(DataFormat::RGBA16F);
    _gNormal->SetOutputFormat(DataFormat::RGBA);
    _gNormal->SetDataType(DataType::Float);
    _gNormal->SetSize(width, height);
    _gNormal->Build();

    _gAlbedoSpec = opfor::Texture2D::Create();
    _gAlbedoSpec->SetTextureParameters(params);
    _gAlbedoSpec->SetInputFormat(DataFormat::RGBA16F);
    _gAlbedoSpec->SetOutputFormat(DataFormat::RGBA);
    _gAlbedoSpec->SetDataType(DataType::Float);
    _gAlbedoSpec->SetSize(width, height);
    _gAlbedoSpec->Build();

    _gMetallicRoughness = opfor::Texture2D::Create();
    _gMetallicRoughness->SetTextureParameters(params);
    _gMetallicRoughness->SetInputFormat(DataFormat::RGBA16F);
    _gMetallicRoughness->SetOutputFormat(DataFormat::RGBA);
    _gMetallicRoughness->SetDataType(DataType::Float);
    _gMetallicRoughness->SetSize(width, height);
    _gMetallicRoughness->Build();

    _gDepth = Texture2D::Create();
    _gDepth->SetTextureParameters(params);
    _gDepth->SetInputFormat(DataFormat::Depth);
    _gDepth->SetOutputFormat(DataFormat::Depth);
    _gDepth->SetDataType(DataType::UnsignedByte);
    _gDepth->SetSize(width, height);
    _gDepth->Build();

    _gBuffer->AttachTexture(_gPosition, FramebufferAttachment::ColorAttachment0);
    _gBuffer->AttachTexture(_gNormal, FramebufferAttachment::ColorAttachment1);
    _gBuffer->AttachTexture(_gAlbedoSpec, FramebufferAttachment::ColorAttachment2);
    _gBuffer->AttachTexture(_gMetallicRoughness, FramebufferAttachment::ColorAttachment3);
    _gBuffer->AttachTexture(_gDepth, FramebufferAttachment::DepthAttachment);

    OP4_CORE_ASSERT(_gBuffer->IsComplete(), "Incomplete Framebuffer!");

    _gBuffer->Unbind();
}

void GBuffer::Resize(unsigned int width, unsigned int height)
{
    _gPosition->SetSize(width, height);
    _gPosition->Build();

    _gNormal->SetSize(width, height);
    _gNormal->Build();

    _gAlbedoSpec->SetSize(width, height);
    _gAlbedoSpec->Build();

    _gMetallicRoughness->SetSize(width, height);
    _gMetallicRoughness->Build();

    _gDepth->SetSize(width, height);
    _gDepth->Build();
}
