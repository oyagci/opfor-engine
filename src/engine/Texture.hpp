#pragma once

#include <string>
#include "lazy.hpp"

class Texture
{
public:
	enum TextureType {
		Tex_Diffuse,
		Tex_Specular,
		Tex_Normal,
		Tex_Bump,
	};
public:
	Texture() = delete;
	Texture(std::string const &name, TextureType type);
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
	TextureType _type;
};
