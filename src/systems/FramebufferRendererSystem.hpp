#pragma once

#include "Framebuffer.hpp"
#include "ecs/System.hpp"
#include <glm/vec3.hpp>

class FramebufferRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Mesh _quad;
	lazy::graphics::Shader _shader;

	opfor::Framebuffer _framebuffer;

	void initQuad()
	{
		std::array<glm::vec3, 4> pos = {
			glm::vec3(-0.5f, -0.5f, 0.0f),
			glm::vec3( 0.5f, -0.5f, 0.0f),
			glm::vec3( 0.5f,  0.5f, 0.0f),
			glm::vec3(-0.5f,  0.5f, 0.0f),
		};

		for (auto p : pos) {
			_quad.addPosition(p);
		}
		_quad.addTriangle(glm::uvec3(0, 1, 2));
		_quad.addTriangle(glm::uvec3(0, 2, 3));
		_quad.build();
	}

public:
	FramebufferRendererSystem() : _framebuffer(opfor::Framebuffer_Type::RW)
	{
		initQuad();
		_shader.addVertexShader("shaders/fb.vs.glsl")
			.addFragmentShader("shaders/fb.fs.glsl")
			.link();

		_shader.bind();
		_shader.setUniform1i("screen_texture", 0);
		_shader.unbind();

		_framebuffer.AttachColorBuffer();
		_framebuffer.AttachDepthBuffer();
	}

	void OnUpdate(float) override
	{
		// first pass
		_framebuffer.Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		// DrawScene();

		// second pass
		_framebuffer.Unbind();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		_shader.bind();
		glDisable(GL_DEPTH_TEST);
//		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};
