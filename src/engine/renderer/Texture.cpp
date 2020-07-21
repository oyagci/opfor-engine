#include "Texture.hpp"
#include "RendererAPI.hpp"
#include "engine/platform/opengl/OpenGLTexture.hpp"

namespace opfor {

UniquePtr<Texture> Texture::Create(TextureType type,
		TextureParameterList params,
		DataFormat inputFormat,
		DataFormat outputFormat,
		DataType dataType,
		size_t width,
		size_t height,
		size_t,
		void *data)
{
	UniquePtr<OpenGLTexture> texture;

	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			texture = MakeUnique<OpenGLTexture>(type, inputFormat, outputFormat, dataType, width, height, data);
			texture->SetParameters(params);
			return texture;
		default:
			OP4_CORE_EXPECT(false, "Unsupported Platform API\n");
	};

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
