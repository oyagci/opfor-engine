#include "Framebuffer.hpp"
#include "RendererAPI.hpp"
#include "backend/opengl/OpenGLFramebuffer.hpp"

namespace opfor
{

UniquePtr<Framebuffer> Framebuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::OpenGL:
        return MakeUnique<OpenGLFramebuffer>();
    case RendererAPI::API::None:
        OP4_CORE_EXPECT(false, "Unsupported Platform!\n");
        break;
    }

    OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
}

} // namespace opfor
