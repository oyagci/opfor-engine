#include "Renderer.hpp"

namespace opfor {

void Renderer::BeginScene()
{
}

void Renderer::EndScene()
{
}

void Renderer::SetClearColor(std::array<float, 4> const &color)
{
	RenderCommand::SetClearColor(std::forward<decltype(color)>(color));
}

void Renderer::Clear()
{
	RenderCommand::Clear();
}

void Renderer::Submit(SharedPtr<VertexArray> const &vertexArray)
{
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
	vertexArray->Unbind();
}

}
