#include "Texture.hpp"
#include "stb_image.h"

Texture::Texture(std::string const &name) : _name(name)
{
	glGenTextures(1, &_glId);
}

Texture::~Texture()
{
	glDeleteTextures(1, &_glId);
}

bool Texture::load(std::string const &path)
{
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nChannel, 0);

	if (data) {
		GLuint format = 3;

		if (_nChannel == 3) {
			format = GL_RGB;
		}
		else if (_nChannel == 4) {
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0,
			format, _width, _height, 0,
			format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		return true;
	}
	return false;
}

void Texture::setParameter(GLenum target, GLenum param, GLenum value)
{
	glBindTexture(GL_TEXTURE_2D, _glId);
	glTexParameteri(target, param, value);
}

void Texture::bind(GLuint textureNumber)
{
	glActiveTexture(textureNumber);
	glBindTexture(GL_TEXTURE_2D, _glId);
}
