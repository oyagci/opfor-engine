#include "Shader.hpp"
#include "RendererAPI.hpp"
#include "engine/platform/opengl/OpenGLShader.hpp"

namespace opfor {

UniquePtr<Shader> Shader::Create()
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLShader>();
		case RendererAPI::API::None:
			OP4_CORE_ASSERT(false, "Unsupported platform!\n");
	}

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
