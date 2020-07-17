#include "Context.hpp"
#include "engine/platform/opengl/OpenGLContext.hpp"

namespace opfor {

UniquePtr<IRendererContext> IRendererContext::Create()
{
	return MakeUnique<OpenGLContext>();
}

}
