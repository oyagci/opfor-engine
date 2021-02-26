#include "OpenGLVertexArray.hpp"
#include "opfor/core/base.hpp"

namespace opfor {

static GLenum ShaderDataTypeToOpenGLBase(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Int:    return GL_UNSIGNED_INT;
		case ShaderDataType::Int2:   return GL_UNSIGNED_INT;
		case ShaderDataType::Int3:   return GL_UNSIGNED_INT;
		case ShaderDataType::Int4:   return GL_UNSIGNED_INT;
		case ShaderDataType::Float:  return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		default: OP4_CORE_EXPECT(false, "Unknown ShaderDataType value!\n");
	};
}

OpenGLVertexArray::OpenGLVertexArray()
{
	glGenVertexArrays(1, &_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteBuffers(1, &_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(SharedPtr<VertexBuffer> vertexBuffer)
{
	glBindVertexArray(_RendererID);

	vertexBuffer->Bind();

	auto const &layout = vertexBuffer->GetLayout();
	auto elements = vertexBuffer->GetLayout().GetElements();

	OP4_CORE_ASSERT(elements.size() > 0, "No elements in buffer layout!\n");

	for (uint32_t i = 0; i < static_cast<uint32_t>(elements.size()); i++) {

		auto const &elem = elements[i];

		uint64_t const offset = elem.Offset;

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			static_cast<GLsizei>(elem.GetComponentCount()),
			ShaderDataTypeToOpenGLBase(elem.Type),
			elem.Normalized,
			layout.GetStride(),
			reinterpret_cast<void*>(offset));
	}

	_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(SharedPtr<IndexBuffer> indexBuffer)
{
	glBindVertexArray(_RendererID);
	indexBuffer->Bind();

	_IndexBuffer = indexBuffer;
}

}
