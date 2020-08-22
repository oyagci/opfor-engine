//
// Created by Marc on 01/06/2018.
//

#include "Shader.hpp"
#include "utils/fileutils.hpp"
#include <vector>

namespace lazy
{
	namespace graphics
	{
		Shader::Shader()
		{}

		Shader::~Shader()
		{
			glDeleteProgram(program);
		}

		GLuint Shader::createShader(const char *sources, GLenum type)
		{
			GLuint shader;
			if ((shader = glCreateShader(type)) == GL_FALSE)
				throw std::runtime_error("Shader error: Unable to create shader !");

			glShaderSource(shader, 1, &sources, NULL);
			glCompileShader(shader);

			GLint status;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE)
			{
				GLint length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
				if (length > 0)
				{
					std::vector<GLchar> msg(length);
					glGetShaderInfoLog(shader, length, &length, msg.data());
					std::cout << " Shader error:\n" << msg.data() << std::endl;
					glDeleteShader(shader);
				}
				return 0;
			}
			return shader;
		}

		Shader &Shader::addVertexShader(const std::string &path)
		{
			if (shaders.find("vertex") != shaders.end())
				return *this;
			shaders["vertex"] = createShader(utils::LoadShader(path).c_str(), GL_VERTEX_SHADER);
			return *this;
		}

		Shader &Shader::addGeometryShader(const std::string &path)
		{
			if (shaders.find("geometry") != shaders.end())
				return *this;
			shaders["geometry"] = createShader(utils::LoadShader(path).c_str(), GL_GEOMETRY_SHADER);

			return *this;
		}

		Shader &Shader::addTesselationShader(const std::string &path)
		{
			if (shaders.find("tesselation") != shaders.end())
				return *this;
			shaders["tesselation"] = createShader(utils::LoadShader(path).c_str(), GL_TESS_CONTROL_SHADER);

			return *this;
		}

		Shader &Shader::addComputeShader(const std::string &path)
		{
			if (shaders.find("compute") != shaders.end())
				return *this;
			shaders["compute"] = createShader(utils::LoadShader(path).c_str(), GL_COMPUTE_SHADER);

			return *this;
		}

		Shader &Shader::addFragmentShader(const std::string &path)
		{
			if (shaders.find("fragment") != shaders.end())
				return *this;
			shaders["fragment"] = createShader(utils::LoadShader(path).c_str(), GL_FRAGMENT_SHADER);
			return *this;
		}

		void Shader::link()
		{
			if ((program = glCreateProgram()) == GL_FALSE)
				throw std::runtime_error("Shader program error: Unable to create shader program !");

			glAttachShader(program, shaders["vertex"]);
			glAttachShader(program, shaders["fragment"]);
			if (shaders.find("geometry") != shaders.end()) {
				glAttachShader(program, shaders["geometry"]);
			}

			glLinkProgram(program);

			GLint result;
			glGetProgramiv(program, GL_LINK_STATUS, &result);

			if (result == GL_FALSE)
			{
				GLint length = 0;
				std::cout << "Shader Link Error" << std::endl;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
				if (length > 0) {
					GLchar *msg = new GLchar[length];
					glGetProgramInfoLog(program, length, &length, msg);
					std::cout << "Shader error:\n" << msg << std::endl;
					glDeleteShader(program);
					delete[] msg;
				}
			}
		}

		bool Shader::isValid()
		{
			glValidateProgram(program);
			GLint result;
			glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
			if (result == GL_FALSE)
			{
				int log_length;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
				if (log_length)
				{
					char *log = new char[log_length];
					glGetProgramInfoLog(program, log_length, nullptr, log);
					std::cout << "Shader error: ";
					std::cout << log;
					delete[] log;
				}
				return false;
			}
			return true;
		}

		GLint Shader::getUniformLocation(const std::string &name)
		{
			if (uniformLocations.find(name) != uniformLocations.end())
				return uniformLocations[name];

			GLint location = glGetUniformLocation(program, name.c_str());
			uniformLocations[name] = location;

			return location;
		}

		void Shader::setUniform1i(const std::string &name, const GLint &v)
		{
			GLint location = getUniformLocation(name);
			glUniform1i(location, v);
		}

		void Shader::setUniform1f(const std::string &name, const GLfloat &v)
		{
			GLint location = getUniformLocation(name);
			glUniform1f(location, v);
		}

		void Shader::setUniform3f(const std::string &name, const glm::vec3 &v)
		{
			GLint location = getUniformLocation(name);
			glUniform3f(location, v.x, v.y, v.z);
		}

		void Shader::setUniform4f(const std::string &name, const glm::vec4 &v)
		{
			GLint location = getUniformLocation(name);
			glUniform4f(location, v.x, v.y, v.z, v.w);
		}

		void Shader::setUniform4x4f(const std::string &name, const glm::mat4 &v)
		{
			GLint location = getUniformLocation(name);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
		}

		void Shader::bind()
		{
			glUseProgram(program);
		}

		void Shader::unbind()
		{
			glUseProgram(0);
		}
	}
}
