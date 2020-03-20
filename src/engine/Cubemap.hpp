#pragma once

#include "lazy.hpp"

class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	Cubemap(const Cubemap &) = delete;
	void operator=(const Cubemap &) = delete;

	void draw();

private:
	void setupTexture();
	void setupMesh();

private:
	GLuint _texture;
	GLuint _vao;
	GLuint _vbo;
};
