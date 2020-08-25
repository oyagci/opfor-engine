#include "OpenGLShader.hpp"

#include <sstream>
#include <fstream>
#include <cstring>
#include <glad/glad.h>

namespace opfor {

OpenGLShader::OpenGLShader(std::string shaderPath)
{
	_RendererID = glCreateProgram();

	std::string source;

	auto opt = ReadShaderSource(shaderPath);
	if (opt) {
		source = *opt;
	}
	else { return ; }

	auto shaderSrcs = ParseShaderSource(source);

	if (shaderSrcs.find(ShaderType::Geometry) != std::end(shaderSrcs)) {
		_GeometryShader = AddShaderFromSource(shaderSrcs[ShaderType::Geometry], GL_GEOMETRY_SHADER);
	}
	if (shaderSrcs.find(ShaderType::Vertex) != std::end(shaderSrcs)) {
		_VertexShader = AddShaderFromSource(shaderSrcs[ShaderType::Vertex], GL_VERTEX_SHADER);
	}
	if (shaderSrcs.find(ShaderType::Fragment) != std::end(shaderSrcs)) {
		_FragmentShader = AddShaderFromSource(shaderSrcs[ShaderType::Fragment], GL_FRAGMENT_SHADER);
	}
	if (shaderSrcs.find(ShaderType::Compute) != std::end(shaderSrcs)) {
		_FragmentShader = AddShaderFromSource(shaderSrcs[ShaderType::Compute], GL_COMPUTE_SHADER);
	}

	Link();
}

OpenGLShader::~OpenGLShader()
{
	if (_RendererID) {
		glDeleteProgram(_RendererID);
	}
}

std::optional<uint32_t> OpenGLShader::AddShaderFromSource(std::string const &src, uint32_t shaderType)
{
	GLuint shader;
	if ((shader = glCreateShader(shaderType)) == GL_FALSE)
		throw std::runtime_error("Shader error: Unable to create shader !");

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
			fmt::print(" Shader error: {}\n", msg.data());
			glDeleteShader(shader);
		}
		return std::nullopt;
	}
	return shader;
}

auto OpenGLShader::ParseShaderSource(std::string const &src) -> std::unordered_map<ShaderType, std::string>
{
	const std::array<std::string, 4> prepocessorKeywords = {
		"vertex",
		"fragment",
		"geometry",
		"compute",
	};
	const std::unordered_map<std::string, ShaderType> shaderTypeMatch = {
		{ "vertex",   ShaderType::Vertex },
		{ "fragment", ShaderType::Fragment },
		{ "pixel",    ShaderType::Fragment },
		{ "geometry", ShaderType::Geometry },
		{ "compute",  ShaderType::Compute },
	};

	ShaderType currentShaderType = ShaderType::None;

	std::unordered_map<ShaderType, std::string> shaders;
	std::string currentShader = "";
	std::string currentKeyword = "";

	ShaderParserState state = ShaderParserState::Normal;

	size_t i = 0;
	while (true) {

		char c = src[i];

		if (state == ShaderParserState::Normal) {

			if (c == '#') {
				state = ShaderParserState::PreprocessorStart;
			}
			else {
				currentShader += c;
				i++;
			}
		}
		else if (state == ShaderParserState::PreprocessorStart) {

			currentKeyword = "";
			state = ShaderParserState::PreprocessorKeyword;
			i++;
		}
		else if (state == ShaderParserState::PreprocessorKeyword) {

			if (!isspace(c)) {
				currentKeyword += c;
				i++;
			}
			else {
				state = ShaderParserState::PreprocessorStop;
			}
		}
		else if (state == ShaderParserState::PreprocessorStop) {

			auto match = std::find(prepocessorKeywords.begin(), prepocessorKeywords.end(), currentKeyword);

			if (match != std::end(prepocessorKeywords)) {
				if (currentShaderType != ShaderType::None) {
					shaders[currentShaderType] = currentShader;
					currentShaderType = ShaderType::None;
				}
				currentShaderType = shaderTypeMatch.at(*match);
				currentShader.clear();
			}
			else {
				currentShader += "#" + currentKeyword;
			}

			state = ShaderParserState::Normal;
		}
		if (src[i] == 0) {
			shaders[currentShaderType] = currentShader;
			break ;
		}
	}

	return shaders;
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

std::optional<uint32_t> OpenGLShader::AddShaderFromPath(std::string const &path, uint32_t shaderType)
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
			fmt::print(" Shader error:\n", msg.data());
			glDeleteShader(shader);
		}
		return std::nullopt;
	}
	return shader;
}

void OpenGLShader::Link()
{
	if (_GeometryShader) { glAttachShader(_RendererID, *_GeometryShader); }
	if (_VertexShader)   { glAttachShader(_RendererID, *_VertexShader); }
	if (_FragmentShader) { glAttachShader(_RendererID, *_FragmentShader); }

	glLinkProgram(_RendererID);

	if (_GeometryShader) { glDeleteShader(*_GeometryShader); }
	if (_VertexShader)   { glDeleteShader(*_VertexShader); }
	if (_FragmentShader) { glDeleteShader(*_FragmentShader); }

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
