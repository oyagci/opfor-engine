#pragma once

#include <string>
#include "lazy.hpp"

class Texture
{
public:
	enum class Target {
		T_2D = GL_TEXTURE_2D,
		T_3D = GL_TEXTURE_3D,
		T_Cubemap = GL_TEXTURE_CUBE_MAP,
	};

private:
	Texture() {};

public:
	Texture(std::string const &name, GLenum target);
	Texture(Texture &&);
	~Texture();

	Texture(GLuint id, std::string const &name, int width, int height, int nchannel, GLenum target)
		: _name(name), _width(width), _height(height), _nChannel(nchannel), _glId(id), _target(target)
	{}

	bool load(std::string const &path);
	void bind(GLuint textureNumber);

	std::string const &getName() const { return _name; }
	int width() const { return _width; }
	int height() const { return _height; }
	int nChannel() const { return _nChannel; }
	GLuint id() const { return _glId; }

	void setParameter(GLenum target, GLenum param, GLenum value);
	void setSRGB(bool srgb) { _srgb = srgb; }

private:
	std::string _name;
	int _width;
	int _height;
	int _nChannel;
	bool _srgb;
	GLuint _glId;
	GLenum _target;
};
