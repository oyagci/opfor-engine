#include "Texture.hpp"
#include "RendererAPI.hpp"
#include "engine/platform/opengl/OpenGLTexture.hpp"

namespace opfor {

UniquePtr<Texture> Texture::Create()
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLTexture>();
		default:
			OP4_CORE_EXPECT(false, "Unsupported Platform API\n");
	};

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
