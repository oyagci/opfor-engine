#include "OpenGLFramebuffer.hpp"
#include "engine/renderer/Texture.hpp"
#include "engine/renderer/Framebuffer.hpp"
#include "engine/renderer/Renderbuffer.hpp"

namespace opfor {

OpenGLFramebuffer::OpenGLFramebuffer()
{
	glGenFramebuffers(1, &_RendererID);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &_RendererID);
}

void OpenGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);
}

void OpenGLFramebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::AttachTexture(SharedPtr<Texture> texture, FramebufferAttachment attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)attachment, GL_TEXTURE_2D, texture->GetRawHandle(), 0);

	_Attachments.insert(attachment);
	UpdateDrawBuffers();

	_Textures.push_back(texture);
}

void OpenGLFramebuffer::AttachRenderbuffer(SharedPtr<Renderbuffer> renderbuffer, RenderbufferAttachment attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)attachment, GL_RENDERBUFFER, renderbuffer->GetRawHandle());

	_Renderbuffers.push_back(renderbuffer);
}

void OpenGLFramebuffer::UpdateDrawBuffers() const
{
	std::vector<GLuint> attachments;

	attachments.reserve(_Attachments.size());

	for (auto const &attach : _Attachments) {
		attachments.push_back((GLuint)attach);
	}

	glDrawBuffers(attachments.size(), attachments.data());
}

bool OpenGLFramebuffer::IsComplete() const
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

}
