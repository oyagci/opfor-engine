#include "VertexArray.hpp"
#include "Renderer.hpp"
#include "platform/opengl/OpenGLVertexArray.hpp"

namespace opfor {

UniquePtr<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			OP4_CORE_EXPECT(false, "RendererAPI::None is not supported!\n");
			break ;
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLVertexArray>();
	}

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
