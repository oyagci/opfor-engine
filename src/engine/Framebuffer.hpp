#pragma once

#include "lazy.hpp"

namespace engine {

enum class Framebuffer_Type {
	R,
	W,
	RW,
};

class Framebuffer
{
private:
	GLuint _id;
	GLuint _bindType;

	GLuint _texture;
	GLuint _depth;

public:
	Framebuffer() = delete;
	Framebuffer(Framebuffer_Type type);
	~Framebuffer();

	Framebuffer(Framebuffer &) = delete;
	void operator=(Framebuffer &) = delete;

	void Bind();
	void Unbind();

	bool IsComplete();

	void AttachColorBuffer();
	void AttachDepthBuffer();

	GLuint GetColorTexture() { return _texture; }
};

}
