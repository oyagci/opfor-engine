#include "Framebuffer.hpp"
#include "Engine.hpp"

namespace engine {

Framebuffer::Framebuffer(Framebuffer_Type type)
{
	glGenFramebuffers(1, &_id);

	switch (type) {
	case Framebuffer_Type::R:
		_bindType = GL_READ_FRAMEBUFFER;
		break;
	case Framebuffer_Type::W:
		_bindType = GL_DRAW_FRAMEBUFFER;
		break;
	case Framebuffer_Type::RW:
		_bindType = GL_FRAMEBUFFER;
		break;
	default:
		throw std::runtime_error("Unreachable");
		break ;
	}
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &_id);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(_bindType, _id);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(_bindType, 0);
}

bool Framebuffer::IsComplete()
{
	bool complete = false;

	Bind();
	complete = glCheckFramebufferStatus(_bindType) == GL_FRAMEBUFFER_COMPLETE;
	Unbind();

	return complete; 
}

void Framebuffer::AttachColorBuffer()
{
	int width = Engine::Instance().GetDisplay()->getWidth();
	int height = Engine::Instance().GetDisplay()->getHeight();

	glGenTextures(1, &_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Bind();
	glFramebufferTexture2D(_bindType, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
	Unbind();
}

void Framebuffer::AttachDepthBuffer()
{
	int width = Engine::Instance().GetDisplay()->getWidth();
	int height = Engine::Instance().GetDisplay()->getHeight();

	glGenRenderbuffers(1, &_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, _depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	Bind();
	glFramebufferRenderbuffer(_bindType, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depth);
	Unbind();
}

}
