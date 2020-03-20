#pragma once

#include <string>
#include "lazy.hpp"

class Texture
{
public:
	Texture() = delete;
	Texture(std::string const &name);
	Texture(Texture &&);
	~Texture();

	bool load(std::string const &path);
	void bind(GLuint textureNumber);

	std::string const &getName() const { return _name; }
	int width() const { return _width; }
	int height() const { return _height; }
	int nChannel() const { return _nChannel; }

	void setParameter(GLenum target, GLenum param, GLenum value);

private:
	std::string _name;
	int _width;
	int _height;
	int _nChannel;

	GLuint _glId;
};
