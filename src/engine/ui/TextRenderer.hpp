#pragma once

#include <lazy.hpp>
#include "Anchor.hpp"

using namespace lazy::graphics;
using namespace anchor;

class TextRenderer
{
public:
//	TextRenderer() = delete;
	TextRenderer(float width = 1280.0f, float height = 720.0f);
	TextRenderer(const TextRenderer &) = delete;

	void operator=(TextRenderer const &) = delete;

	void setup();
	void drawText(std::string text, GLfloat scale, glm::vec3 color, Anchor anchor = Anchor::BottomLeft);

private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ubo;
	Shader _shader;
	int _width;
	int _height;

	struct Character {
		GLuint texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
	};
	std::map<GLchar, Character> _characters;

};
