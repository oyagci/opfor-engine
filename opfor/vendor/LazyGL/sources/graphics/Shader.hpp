//
// Created by Marc on 01/06/2018.
//

#pragma once

#include <iostream>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace lazy
{
	namespace graphics
	{
		class Shader
		{
		private:
			GLuint							program;
			std::map<std::string, GLint>	uniformLocations;
			std::map<std::string, GLuint>	shaders;

			GLuint createShader(const char *sources, GLenum type);

		public:
			Shader();
			~Shader();

			Shader &addVertexShader(const std::string &path);
			Shader &addGeometryShader(const std::string &path);
			Shader &addTesselationShader(const std::string &path);
			Shader &addComputeShader(const std::string &path);
			Shader &addFragmentShader(const std::string &path);

			void link();

			GLint getUniformLocation(const std::string &name);
			
			void setUniform1i(const std::string &name, const GLint &v);
			void setUniform1f(const std::string &name, const GLfloat &v);
			void setUniform3f(const std::string &name, const glm::vec3 &v);
			void setUniform4f(const std::string &name, const glm::vec4 &v);
			void setUniform4x4f(const std::string &name, const glm::mat4 &v);

			void bind();
			void unbind();

			bool isValid();

			GLuint	getProgram() const { return program; }
		};
	}
}
