#include "OpenGLShader.hpp"

#include <sstream>
#include <fstream>
#include "lazy.hpp"

namespace opfor {

OpenGLShader::OpenGLShader()
{
	_RendererID = glCreateProgram();
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(_RendererID);
}

std::optional<std::string> OpenGLShader::ReadShaderSource(std::string const &path)
{
	std::stringstream result;
	std::ifstream in(path);

	if (in.is_open() && !in.bad())
	{
		result << in.rdbuf();
		in.close();
		return result.str();
	}

	return std::nullopt;
}

std::optional<uint32_t> OpenGLShader::CreateShader(std::string const &path, uint32_t shaderType)
{
	GLuint shader;
	if ((shader = glCreateShader(shaderType)) == GL_FALSE)
		throw std::runtime_error("Shader error: Unable to create shader !");

	std::string src = ReadShaderSource(path).value();

	char const *c_src = src.c_str();

	glShaderSource(shader, 1, &c_src, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
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

void OpenGLShader::AddGeometryShader(std::string const &path)
{
	_GeometryShader = CreateShader(path, GL_GEOMETRY_SHADER);
}

void OpenGLShader::AddVertexShader(std::string const &path)
{
	_VertexShader = CreateShader(path, GL_VERTEX_SHADER);
}

void OpenGLShader::AddFragmentShader(std::string const &path)
{
	_FragmentShader = CreateShader(path, GL_FRAGMENT_SHADER);
}

void OpenGLShader::Link()
{
	if (_GeometryShader.has_value()) {
		glAttachShader(_RendererID, *_GeometryShader);
	}

	if (_VertexShader.has_value()) {
		glAttachShader(_RendererID, *_VertexShader);
	}

	if (_FragmentShader.has_value()) {
		glAttachShader(_RendererID, *_FragmentShader);
	}

	glLinkProgram(_RendererID);

	GLint result;
	glGetProgramiv(_RendererID, GL_LINK_STATUS, &result);

	if (result == GL_FALSE) {

		OP4_CORE_ERROR("Could not link shader!\n");

		GLint length = 0;
		glGetProgramiv(_RendererID, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			GLchar *msg = new GLchar[length];
			glGetProgramInfoLog(_RendererID, length, &length, msg);

			OP4_CORE_ERROR("Shader Error:\n");
			OP4_CORE_ERROR("============\n");
			OP4_CORE_ERROR("{}\n", msg);

			glDeleteShader(_RendererID);
			delete[] msg;
		}
	}
}

uint32_t OpenGLShader::FindUniformLocation(std::string name)
{
	auto loc = _UniformLocations.find(name);

	if (loc != _UniformLocations.end()) {
		return loc->second;
	}
	else {
		auto loc = glGetUniformLocation(_RendererID, name.c_str());
		_UniformLocations[name] = loc;

		return loc;
	}
}

void OpenGLShader::SetUniform(std::string const &name, size_t value)
{
	auto loc = FindUniformLocation(name);
	glUniform1i(loc, value);
}

void OpenGLShader::SetUniform(std::string const &name, int32_t value)
{
	auto loc = FindUniformLocation(name);
	glUniform1i(loc, value);
}

void OpenGLShader::SetUniform(std::string const &name, uint32_t value)
{
	auto loc = FindUniformLocation(name);
	glUniform1ui(loc, value);
}

void OpenGLShader::SetUniform(std::string const &name, float value)
{
	auto loc = FindUniformLocation(name);
	glUniform1f(loc, value);
}

void OpenGLShader::SetUniform(std::string const &name, glm::vec3 value)
{
	auto loc = FindUniformLocation(name);
	glUniform3f(loc, value.x, value.y, value.z);
}

void OpenGLShader::SetUniform(std::string const &name, glm::vec4 value)
{
	auto loc = FindUniformLocation(name);
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void OpenGLShader::SetUniform(std::string const &name, glm::mat3 value)
{
	auto loc = FindUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
}

void OpenGLShader::SetUniform(std::string const &name, glm::mat4 value)
{
	auto loc = FindUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}

void OpenGLShader::SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size)
{
	auto dataSize = matrices.size();

	if (size) {
		dataSize = *size;
	}

	auto loc = FindUniformLocation(name);
	glUniformMatrix4fv(loc, dataSize, GL_FALSE, (float *)matrices.data());
}

void OpenGLShader::SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size)
{
	auto dataSize = vectors.size();

	if (size) {
		dataSize = *size;
	}

	auto loc = FindUniformLocation(name);
	glUniform3fv(loc, dataSize, (float *)vectors.data());
}

void OpenGLShader::Bind()
{
	glUseProgram(_RendererID);
}

void OpenGLShader::Unbind()
{
	glUseProgram(0);
}

}
