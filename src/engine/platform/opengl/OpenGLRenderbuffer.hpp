#pragma once

#include "engine/renderer/Renderbuffer.hpp"

namespace opfor {

class OpenGLRenderbuffer : public Renderbuffer
{
private:
	uint32_t _RendererID;

public:
	OpenGLRenderbuffer();
	virtual ~OpenGLRenderbuffer();

	void Bind() const override;
	void Unbind() const override;

	void AttachDepthComponent(std::array<unsigned int, 2> size) override;

	uint32_t GetRawHandle() const override { return _RendererID; }
};

}
