#include "Renderbuffer.hpp"
#include "RendererAPI.hpp"
#include "engine/platform/opengl/OpenGLRenderbuffer.hpp"

namespace opfor {

UniquePtr<Renderbuffer> Renderbuffer::Create()
{
	switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None:
			OP4_CORE_EXPECT(false, "Unsupported Platform API!\n");
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLRenderbuffer>();
	};

	OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

}
