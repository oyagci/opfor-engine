#include "Viewport.hpp"

namespace opfor
{

Viewport::Viewport(unsigned int width, unsigned int height)
{
    _Framebuffer = Framebuffer::Create();
    _Texture = Texture2D::Create();
    _Texture->SetSize(1920, 1080);
    _Texture->SetInputFormat(DataFormat::RGB);
    _Texture->SetOutputFormat(DataFormat::RGB);
    _Texture->SetDataType(DataType::UnsignedInt);
    _Texture->SetTextureParameters({
        {TextureParameterType::MagnifyFilter, TextureParameterValue::Nearest},
        {TextureParameterType::MignifyFilter, TextureParameterValue::Nearest},
    });
    _Texture->Build();
    _Framebuffer->AttachTexture(_Texture, FramebufferAttachment::ColorAttachment0);

    _Size = {width, height};
}

void Viewport::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);

    dispatcher.DispatchIf<ViewportResizeEvent>(std::bind(&Viewport::OnViewportResized, this, std::placeholders::_1));
}

bool Viewport::OnViewportResized(ViewportResizeEvent &e)
{
    _Texture->Bind(TextureUnit::Texture0);
    _Texture->SetSize(e.GetWidth(), e.GetHeight());
    _Texture->Build();

    _Size = {e.GetWidth(), e.GetHeight()};

    return false;
}
} // namespace opfor