#include "Texture.hpp"
#include "stb_image.h"
#include <fmt/format.h>

Texture::Texture(std::string const &name, GLenum target) : _name(name), _target(target)
{
	glGenTextures(1, &_glId);
}

Texture::Texture(Texture &&rhs)
{
	_name = rhs._name;
	_width = rhs._width;
	_height = rhs._height;
	_nChannel = rhs._nChannel;
	_glId = rhs._glId;
	_target = rhs._target;

	rhs._glId = 0;
}

Texture::~Texture()
{
	glDeleteTextures(1, &_glId);
}

bool Texture::load(std::string const &path)
{
	stbi_set_flip_vertically_on_load(0);
	unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nChannel, 0);

	if (data) {
		GLuint format = 3;

		if (_nChannel == 3) {
			if (_srgb) {
				format = GL_SRGB;
			}
			else {
				format = GL_RGB;
			}
		}
		else if (_nChannel == 4) {
			if (_srgb) {
				format = GL_SRGB_ALPHA;
			}
			else {
				format = GL_RGBA;
			}
		}

		float outFormat;
		if      (format == GL_SRGB) { outFormat = GL_RGB; }
		else if (format == GL_SRGB_ALPHA) { outFormat = GL_RGBA; }
		else { outFormat = format; }

		glTexImage2D(GL_TEXTURE_2D, 0,
			format, _width, _height, 0,
			outFormat, GL_UNSIGNED_BYTE, data);
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
	glActiveTexture(GL_TEXTURE0 + textureNumber);
	glBindTexture(_target, _glId);
}
