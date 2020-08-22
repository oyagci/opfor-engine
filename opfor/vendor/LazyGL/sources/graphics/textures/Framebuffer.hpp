#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace lazy {
	namespace graphics {

class Framebuffer
{
private:
    int _width;
    int _height;

    GLuint              _fbo;
    GLuint              _rbo;
    std::vector<GLuint> _cbos;
    GLuint              _dbo;

public:
    Framebuffer();
    ~Framebuffer();

    void setSize(int width, int height);

    void clean();
    void genFramebuffer();
    void genRenderbuffer(GLuint type, GLuint attachment);
    void genColorTexture(GLuint internalFormat, GLint format, GLuint type, GLuint filterMode, GLuint wrapMode);
    void genDepthTexture(GLuint filterMode, GLuint wrapMode);
    void drawBuffers(std::vector<GLenum> attachments);

    void bind();
    void unbind();

    GLuint getFramebuffer() const { return _fbo; }
    GLuint getRenderbuffer() const { return _rbo; }
    GLuint getColorTexture(int id) const { return _cbos[id]; }
    GLuint getDepthTexture() const { return _dbo; }
};

	}
}
