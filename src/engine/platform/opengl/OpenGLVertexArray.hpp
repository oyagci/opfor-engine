#include "engine/renderer/VertexArray.hpp"

namespace opfor {

class OpenGLVertexArray : public VertexArray
{
private:
	uint32_t _RendererID;

	std::vector<SharedPtr<VertexBuffer>> _VertexBuffers;
	SharedPtr<IndexBuffer> _IndexBuffer;

public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray();

	void Bind() const override;
	void Unbind() const override;

	void AddVertexBuffer(SharedPtr<VertexBuffer>) override;
	void SetIndexBuffer(SharedPtr<IndexBuffer>) override;

	IndexBuffer const *GetIndexBuffer() const override { return _IndexBuffer.get(); }
};

}
