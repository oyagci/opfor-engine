#pragma once

#include "opfor/renderer/Buffer.hpp"

namespace opfor {

class OpenGLVertexBuffer : public VertexBuffer
{
private:
	uint32_t _RendererID;
	BufferLayout _Layout;

public:
	OpenGLVertexBuffer(float *vertices, size_t size);
	virtual ~OpenGLVertexBuffer();

	void Bind() const override;
	void Unbind() const override;

	void SetLayout(BufferLayout layout) override;
	BufferLayout const &GetLayout() const override;
};

class OpenGLIndexBuffer : public IndexBuffer
{
private:
	uint32_t _RendererID;
	size_t _Count;

public:
	OpenGLIndexBuffer(uint32_t *indices, size_t count);
	virtual ~OpenGLIndexBuffer();

	void Bind() const override;
	void Unbind() const override;

	size_t GetCount() const override;
};

}
