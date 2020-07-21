#include "VertexArray.hpp"
#include "Renderer.hpp"
#include "engine/platform/opengl/OpenGLVertexArray.hpp"

namespace opfor {

UniquePtr<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI()) {
		case RendererAPI::None:
			OP4_CORE_EXPECT(false, "RendererAPI::None is not supported!\n");
			break ;
		case RendererAPI::OpenGL:
			return MakeUnique<OpenGLVertexArray>();
	}

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
