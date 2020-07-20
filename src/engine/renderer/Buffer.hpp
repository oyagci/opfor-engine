#pragma once

#include <memory>
#include "engine/core/base.hpp"
#include "lazy.hpp"

namespace opfor {

enum class ShaderDataType {
	None = 0, Int, Int2, Int3, Int4, Float, Float2, Float3, Float4
};

static uint32_t GetShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Int:    return 4 * 1;
		case ShaderDataType::Int2:   return 4 * 2;
		case ShaderDataType::Int3:   return 4 * 3;
		case ShaderDataType::Int4:   return 4 * 4;
		case ShaderDataType::Float:  return 4 * 1;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;
		default: OP4_CORE_EXPECT(false, "Unknown ShaderDataType value!\n");
	};
}

inline GLenum ShaderDataTypeToOpenGLBase(ShaderDataType type)
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

struct BufferElement
{
	std::string Name;
	ShaderDataType Type;
	uint32_t Size;
	uint32_t Offset;
	bool Normalized;

	BufferElement(ShaderDataType type, std::string name, bool normalized = false)
		: Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{
	}

	size_t GetComponentCount() const
	{
		switch (Type)
		{
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			default: OP4_CORE_EXPECT(false, "Unknown ShaderDataType value!\n");
		};
	}
};

class BufferLayout
{
private:
	std::vector<BufferElement> _Elements;
	uint32_t _Stride = 0;

	void CalcOffsetAndStride()
	{
		size_t offset = 0;
		_Stride = 0;
		for (auto &element : _Elements) {
			element.Offset = offset;
			_Stride += element.Size;
			offset += element.Size;
		}
	}

public:
	BufferLayout() {}
	BufferLayout(std::initializer_list<BufferElement> const &elements)
		: _Elements(elements)
	{
		CalcOffsetAndStride();
	}
	BufferLayout(BufferLayout const &other) = default;
	~BufferLayout() = default;

	BufferLayout &operator=(BufferLayout const &other) = default;

	auto const &GetElements() const { return _Elements; }
	auto GetStride() const { return _Stride; }
	auto GetSize() const { return _Stride * _Elements.size(); }

	// Iterators
	// =========
	std::vector<BufferElement>::const_iterator begin() const { return _Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return _Elements.end(); }
	std::vector<BufferElement>::iterator begin() { return _Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return _Elements.end(); }
};

class VertexBuffer
{
public:
	virtual ~VertexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
	virtual void SetLayout(BufferLayout layout) = 0;

	virtual BufferLayout const &GetLayout() const = 0;

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
