#include "OpenGLRendererAPI.hpp"

namespace opfor {

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(std::array<float, 4> const color)
	{
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void OpenGLRendererAPI::DrawIndexed(SharedPtr<VertexArray> const &vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(),
			GL_UNSIGNED_INT, nullptr);
	}

}
