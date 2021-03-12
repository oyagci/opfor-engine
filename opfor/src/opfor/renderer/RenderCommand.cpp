#include "RenderCommand.hpp"

namespace opfor
{

UniquePtr<RendererAPI> RenderCommand::_API = RendererAPI::Create();

}
