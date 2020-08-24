#pragma once

#include "opfor/core/base.hpp"
#include <array>

namespace opfor {

class Renderbuffer
{
private:
public:
	virtual ~Renderbuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void AttachDepthComponent(std::array<unsigned int, 2> size) = 0;

	virtual void Resize(unsigned int width, unsigned int height) = 0;

	virtual uint32_t GetRawHandle() const = 0;

	static UniquePtr<Renderbuffer> Create();
};

}
