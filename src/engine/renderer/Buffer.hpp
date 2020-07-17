#pragma once

#include <memory>

namespace opfor {

class VertexBuffer
{
public:
	virtual ~VertexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	static UniquePtr<VertexBuffer> Create(float *vertices, size_t size);
};

class IndexBuffer
{
public:
	virtual ~IndexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual size_t GetCount() const = 0;

	static UniquePtr<IndexBuffer> Create(uint32_t *indices, size_t size);
};

}
