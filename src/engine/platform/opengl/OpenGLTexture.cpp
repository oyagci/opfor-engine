#include "OpenGLTexture.hpp"

namespace opfor {

void OpenGLTexture::Build()
{
	switch (GetTextureType()) {
		case TextureType::Tex2D:
			glBindTexture(GL_TEXTURE_2D, _RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, (GLint)GetInputFormat(), GetSize().x, GetSize().y, 0, (GLint)GetOutputFormat(),
				(GLenum)GetDataType(), GetTextureData());
			break ;
		case TextureType::TexCubemap:
			glBindTexture(GL_TEXTURE_CUBE_MAP, _RendererID);
			for (size_t face = 0; face < 6; face++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
					(GLint)GetInputFormat(), GetSize().x, GetSize().y, 0,
					(GLint)GetOutputFormat(), (GLenum)GetDataType(), GetTextureData());
			}
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			break ;

		default:
			OP4_CORE_ASSERT(false, "Wrong/Unsupported TextureType given!\n");
			return ;
	};
}

OpenGLTexture::OpenGLTexture()
{
	glGenTextures(1, &_RendererID);
}

OpenGLTexture::~OpenGLTexture()
{
	glDeleteTextures(1, &_RendererID);
}

void OpenGLTexture::Bind(TextureUnit unit)
{
	glActiveTexture((GLenum)unit);
	glBindTexture((GLenum)GetTextureType(), (GLuint)_RendererID);
	_BoundUnit = unit;
}

void OpenGLTexture::Unbind()
{
	if (_BoundUnit) {
		glActiveTexture((GLenum)_BoundUnit.value());
		glBindTexture((GLenum)GetTextureType(), 0);
		_BoundUnit.reset();
	}
}

}
