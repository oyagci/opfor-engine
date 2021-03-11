#include "RenderCommand.hpp"
#include "backend/opengl/OpenGLRendererAPI.hpp"

namespace opfor
{

UniquePtr<RendererAPI> RenderCommand::_API = MakeUnique<OpenGLRendererAPI>();

}
