#include "TextRenderer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace anchor;

TextRenderer::TextRenderer(float width, float height)
{
	_width = width;
	_height = height;
	setup();
}

void TextRenderer::setup()
{
	GLfloat texCoords [6][2] = {
		{ 0.0, 0.0 },
		{ 0.0, 1.0 },
		{ 1.0, 1.0 },

		{ 0.0, 0.0 },
		{ 1.0, 1.0 },
		{ 1.0, 0.0 }
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ubo);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, _ubo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	FT_Library lib;
	if (FT_Init_FreeType(&lib)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType library" << std::endl;
		return ;
	}
	FT_Face face;
	if (FT_New_Face(lib, "./fonts/MinecraftRegular-Bmg3.otf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return ;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue ;
		}
		GLuint texture;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<GLuint>(face->glyph->advance.x)
		};
		_characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

void TextRenderer::drawText(std::string text, GLfloat scale, glm::vec3 color, Anchor anchor)
{
	glm::vec2 pos(0.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(_vao);

	float textWidth = 0.0f;
	float maxHeight = 0.0f;
	for (auto &c : text) {
        Character const &ch = _characters[c];
		textWidth += (ch.advance >> 6) * scale;
		maxHeight = ch.bearing.y * scale > maxHeight ? ch.bearing.y * scale : maxHeight;
	}

	// Calculate anchor offset to align text to the desired side
	glm::vec2 anchorOffset = calculateOffset(anchor, glm::vec2(textWidth, maxHeight));
	pos += anchorOffset;

	for (auto &c : text)
	{
        Character const &ch = _characters[c];

        GLfloat xpos = pos.x + ch.bearing.x * scale;
        GLfloat ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][2] = {
            { xpos,     ypos + h, },
            { xpos,     ypos,     },
            { xpos + w, ypos,     },

            { xpos,     ypos + h, },
            { xpos + w, ypos,     },
            { xpos + w, ypos + h, },
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
