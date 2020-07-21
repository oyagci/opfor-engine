#include "OpenGLTexture.hpp"

namespace opfor {

OpenGLTexture::OpenGLTexture(TextureType type, DataFormat input, DataFormat output,
	DataType dataType, size_t width, size_t height, void *data)
	: _Type(type)
{
	glGenTextures(1, &_RendererID);

	switch (type) {
		case TextureType::Tex2D:
			glBindTexture(GL_TEXTURE_2D, _RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, (GLint)input, width, height, 0, (GLint)output,
				(GLenum)dataType, data);
			break ;
		default:
			OP4_CORE_ASSERT(false, "Wrong/Unsupported TextureType given!\n");
			return ;
	};
}

OpenGLTexture::~OpenGLTexture()
{
	glDeleteTextures(1, &_RendererID);
}

void OpenGLTexture::Bind(TextureUnit unit)
{
	glActiveTexture((GLenum)unit);
	glBindTexture((GLenum)_Type, (GLuint)_RendererID);
	_BoundUnit = unit;
}

void OpenGLTexture::Unbind()
{
	if (_BoundUnit) {
		glActiveTexture((GLenum)_BoundUnit.value());
		glBindTexture((GLenum)_Type, 0);
		_BoundUnit.reset();
	}
}

void OpenGLTexture::SetParameter(TextureParameter param)
{
	glBindTexture((GLenum)_Type, _RendererID);
	glTexParameteri((GLenum)_Type, (GLenum)param.first, (GLint)param.second);
	glBindTexture((GLenum)_Type, _RendererID);
}

void OpenGLTexture::SetParameters(TextureParameterList params)
{
	glBindTexture((GLenum)_Type, _RendererID);
	for (auto const &p : params) {
		glTexParameteri((GLenum)_Type, (GLenum)p.first, (GLint)p.second);
	}
	glBindTexture((GLenum)_Type, _RendererID);
}

}
