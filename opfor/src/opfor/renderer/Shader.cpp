#include "Shader.hpp"
#include "RendererAPI.hpp"
#include "backend/opengl/OpenGLShader.hpp"

namespace opfor {

UniquePtr<Shader> Shader::Create(std::string const &source)
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLShader>(source);
		case RendererAPI::API::None:
			OP4_CORE_ASSERT(false, "Unsupported platform!\n");
	}

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
