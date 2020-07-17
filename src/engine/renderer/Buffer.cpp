#include "Buffer.hpp"
#include "Renderer.hpp"
#include <fmt/format.h>
#include "engine/platform/opengl/OpenGLBuffer.hpp"

namespace opfor {

	UniquePtr<VertexBuffer> VertexBuffer::Create(float *vertices, size_t size)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::None:
				fmt::print("RendererAPI::None is not supported!");
				abort();
			case RendererAPI::OpenGL:
				return MakeUnique<OpenGLVertexBuffer>(vertices, size);
		}

		OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
	}

	UniquePtr<IndexBuffer> IndexBuffer::Create(uint32_t *indices, size_t size)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::None:
				OP4_CORE_EXPECT(false, "RendererAPI::None is not supported!\n");
			case RendererAPI::OpenGL:
				return MakeUnique<OpenGLIndexBuffer>(indices, size);
		}

		OP4_CORE_UNREACHABLE("{}\n", __PRETTY_FUNCTION__);
	}

}
