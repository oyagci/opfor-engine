#include "RenderCommand.hpp"
#include "opfor/platform/opengl/OpenGLRendererAPI.hpp"

namespace opfor {

UniquePtr<RendererAPI> RenderCommand::_API = MakeUnique<OpenGLRendererAPI>();

}
