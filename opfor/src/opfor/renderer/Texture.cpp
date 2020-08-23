#include "renderer/Texture.hpp"
#include "RendererAPI.hpp"
#include "platform/opengl/OpenGLTexture.hpp"

namespace opfor {

UniquePtr<Texture2D> Texture2D::Create()
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLTexture2D>();
		default:
			OP4_CORE_EXPECT(false, "Unsupported Platform API\n");
	};

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

UniquePtr<TextureCubemap> TextureCubemap::Create()
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLTextureCubemap>();
		default:
			OP4_CORE_EXPECT(false, "Unsupported Platform API\n");
	};

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

// Why do I need to define in the source file? Who knows...
Texture2D::~Texture2D() = default;

}
