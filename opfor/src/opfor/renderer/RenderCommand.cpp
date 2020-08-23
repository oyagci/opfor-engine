#include "RenderCommand.hpp"
#include "platform/opengl/OpenGLRendererAPI.hpp"

namespace opfor {

UniquePtr<RendererAPI> RenderCommand::_API = MakeUnique<OpenGLRendererAPI>();

}
