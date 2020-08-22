#include "OpenGLTexture.hpp"
#include <glad/glad.h>

namespace opfor {

// OpenGLTexture2D
// ===============

void OpenGLTexture2D::Build()
{
	GLint inputFormat = (GLint)GetInputFormat();

	if (IsSRGB()) {
		if (GetInputFormat() == DataFormat::RGB) {
			inputFormat = GL_SRGB;
		}
		else if (GetInputFormat() == DataFormat::RGBA) {
			inputFormat = GL_SRGB_ALPHA;
		}
	}

	glBindTexture(GL_TEXTURE_2D, _RendererID);

	glTexImage2D(GL_TEXTURE_2D, 0, (GLint)inputFormat, GetSize().x, GetSize().y, 0, (GLint)GetOutputFormat(),
		(GLenum)GetDataType(), _TextureData);

	ApplyParameters();

	if (ShouldGenerateMipmap()) {
		glGenerateMipmap((GLenum)GetTextureType());
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture2D::OpenGLTexture2D()
{
	SetTextureType(TextureType::Tex2D);
	glGenTextures(1, &_RendererID);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &_RendererID);
}

void OpenGLTexture2D::Bind(TextureUnit unit)
{
	glActiveTexture((GLenum)unit);
	glBindTexture(GL_TEXTURE_2D, (GLuint)_RendererID);
}

void OpenGLTexture2D::ApplyParameters()
{
	for (auto const &param : GetTextureParameters()) {
		glTexParameteri(GL_TEXTURE_2D, (GLenum)param.first, (GLint)param.second);
	}
}

void OpenGLTexture2D::SetData(void *data)
{
	_TextureData = data;
}

// OpenGLTextureCubemap
// ====================

OpenGLTextureCubemap::OpenGLTextureCubemap()
{
	SetTextureType(TextureType::TexCubemap);
	glGenTextures(1, &_RendererID);
}

OpenGLTextureCubemap::~OpenGLTextureCubemap()
{
	glDeleteTextures(1, &_RendererID);
}

void OpenGLTextureCubemap::Bind(TextureUnit unit)
{
	glActiveTexture((GLenum)unit);
	glBindTexture((GLenum)GL_TEXTURE_CUBE_MAP, _RendererID);
}

void OpenGLTextureCubemap::Build()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, _RendererID);
	ApplyParameters();
	if (ShouldGenerateMipmap()) {
		glGenerateMipmap((GLenum)GetTextureType());
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLTextureCubemap::ApplyParameters()
{
	for (auto const &param : GetTextureParameters()) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, (GLenum)param.first, (GLint)param.second);
	}
}

void OpenGLTextureCubemap::SetFaceData(CubemapFace face, ImageLoader::Image image)
{
	glTexImage2D((GLenum)face, 0, (GLint)GetInputFormat(),
		image.width, image.height, 0, (GLint)GetOutputFormat(), (GLenum)GetDataType(),
		image.data.get());
}

}
