#include "RenderCommand.hpp"
#include "engine/platform/opengl/OpenGLRendererAPI.hpp"

namespace opfor {

UniquePtr<RendererAPI> RenderCommand::_API = MakeUnique<OpenGLRendererAPI>();

}
