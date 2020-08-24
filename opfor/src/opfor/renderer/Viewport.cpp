#include "Viewport.hpp"

namespace opfor {

	Viewport::Viewport(unsigned int width, unsigned int height)
	{
		_Framebuffer = opfor::Framebuffer::Create();
		_Texture = opfor::Texture2D::Create();
		_Texture->SetSize(1920, 1080);
		_Texture->SetInputFormat(opfor::DataFormat::RGB);
		_Texture->SetOutputFormat(opfor::DataFormat::RGB);
		_Texture->SetDataType(opfor::DataType::UnsignedInt);
		_Texture->SetTextureParameters({
			{ opfor::TextureParameterType::MagnifyFilter, opfor::TextureParameterValue::Nearest },
			{ opfor::TextureParameterType::MignifyFilter, opfor::TextureParameterValue::Nearest },
			});
		_Texture->Build();
		_Framebuffer->AttachTexture(_Texture, opfor::FramebufferAttachment::ColorAttachment0);

		_Size = { width, height };
	}

	void Viewport::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.DispatchIf<ViewportResizeEvent>(std::bind(&Viewport::OnViewportResized, this, std::placeholders::_1));
	}

	bool Viewport::OnViewportResized(ViewportResizeEvent &e)
	{
		_Texture->Bind(opfor::TextureUnit::Texture0);
		_Texture->SetSize(e.GetWidth(), e.GetHeight());
		_Texture->Build();
		
		_Size = { e.GetWidth(), e.GetHeight() };
		
		return false;
	}
}