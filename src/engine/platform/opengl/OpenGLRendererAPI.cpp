#include "OpenGLRendererAPI.hpp"
#include "engine/renderer/VertexArray.hpp"
#include "engine/renderer/Framebuffer.hpp"
#include "engine/renderer/Texture.hpp"
#include "engine/renderer/Shader.hpp"
#include "lazy.hpp"

namespace opfor {

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(std::array<float, 4> const color)
	{
		glClearColor(color[0], color[1], color[2], color[3]);
	}

	void OpenGLRendererAPI::DrawIndexed(SharedPtr<VertexArray> const &vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(),
			GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::PushFramebuffer(SharedPtr<Framebuffer> const &fb)
	{
		int32_t prevRead = 0;
		int32_t prevDraw = 0;

		// Save previous read/draw buffers
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevRead);
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDraw);

		_prevReadBuffers.push_back(prevRead);
		_prevDrawBuffers.push_back(prevDraw);

		glBindFramebuffer(GL_FRAMEBUFFER, fb->GetRawHandle());
	}

	void OpenGLRendererAPI::PopFramebuffer()
	{
		uint32_t prevRead = _prevReadBuffers.back();
		uint32_t prevDraw = _prevDrawBuffers.back();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, prevRead);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDraw);

		_prevDrawBuffers.pop_back();
		_prevReadBuffers.pop_back();
	}

	void OpenGLRendererAPI::CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &src, CopyTarget target) const
	{
		src->CopyToDefault(target);
	}

	void OpenGLRendererAPI::CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src, CopyTarget target) const
	{
		src->CopyTo(target, *dst);
	}

	void OpenGLRendererAPI::PushCapability(RendererCaps cap, bool enable)
	{
		if (enable) {
			glEnable((GLenum)cap);
		}
		else {
			glDisable((GLenum)cap);
		}

		_capStates[cap].push_back(enable);
	}

	void OpenGLRendererAPI::PopCapability(RendererCaps cap)
	{
		if (_capStates[cap].size() > 0) {
			int32_t prevState = _capStates[cap].back();

			if (prevState) {
				glEnable((GLenum)cap);
			}
			else {
				glDisable((GLenum)cap);
			}

			_capStates[cap].pop_back();
		}
	}

	void OpenGLRendererAPI::PushTexture(SharedPtr<Texture> const &texture, TextureUnit unit)
	{
		int32_t prevTexture = 0;
		TextureType type = texture->GetType();

		glActiveTexture((GLenum)unit);

		if (type == TextureType::Tex1D) {
			glGetIntegerv(GL_TEXTURE_BINDING_1D, &prevTexture);
		}
		else if (type == TextureType::Tex2D) {
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
		}
		else if (type == TextureType::Tex3D) {
			glGetIntegerv(GL_TEXTURE_BINDING_3D, &prevTexture);
		}
		else if (type == TextureType::TexCubemap) {
			glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &prevTexture);
		}

		_prevTextureUnits[unit].push_back({ type, prevTexture });

		glBindTexture((GLenum)type, texture->GetRawHandle());
	}

	void OpenGLRendererAPI::PopTexture(TextureUnit unit)
	{
		auto prevTexture = _prevTextureUnits[unit].back();

		glActiveTexture((GLenum)unit);
		glBindTexture((GLenum)prevTexture.first, (GLuint)prevTexture.second);

		_prevTextureUnits[unit].pop_back();
	}

	void OpenGLRendererAPI::PushShader(SharedPtr<Shader> const &shader)
	{
		int32_t prevProgram = 0;

		glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
		_prevShaders.push_back(prevProgram);

		glUseProgram(shader->GetRawHandle());
	}

	void OpenGLRendererAPI::PopShader()
	{
		auto prevShader = _prevShaders.back();

		glUseProgram(prevShader);

		_prevShaders.pop_back();
	}


	uint32_t OpenGLRendererAPI::FindUniformLocation(std::string name)
	{
		auto loc = _UniformLocations.find(name);

		if (loc != _UniformLocations.end()) {
			return loc->second;
		}
		else {
			auto currentProgram = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
			auto loc = glGetUniformLocation(currentProgram, name.c_str());
			_UniformLocations[name] = loc;

			return loc;
		}
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, size_t value)
	{
		auto loc = FindUniformLocation(name);
		glUniform1i(loc, value);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, int32_t value)
	{
		auto loc = FindUniformLocation(name);
		glUniform1i(loc, value);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, uint32_t value)
	{
		auto loc = FindUniformLocation(name);
		glUniform1ui(loc, value);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, float value)
	{
		auto loc = FindUniformLocation(name);
		glUniform1f(loc, value);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, glm::vec3 value)
	{
		auto loc = FindUniformLocation(name);
		glUniform3f(loc, value.x, value.y, value.z);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, glm::vec4 value)
	{
		auto loc = FindUniformLocation(name);
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, glm::mat3 value)
	{
		auto loc = FindUniformLocation(name);
		glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, glm::mat4 value)
	{
		auto loc = FindUniformLocation(name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, std::vector<glm::mat4> matrices, std::optional<size_t> size)
	{
		auto dataSize = matrices.size();

		if (size) {
			dataSize = *size;
		}

		auto loc = FindUniformLocation(name);
		glUniformMatrix4fv(loc, dataSize, GL_FALSE, (float *)matrices.data());
	}

	void OpenGLRendererAPI::SetUniform(std::string const &name, std::vector<glm::vec3> vectors, std::optional<size_t> size)
	{
		auto dataSize = vectors.size();

		if (size) {
			dataSize = *size;
		}

		auto loc = FindUniformLocation(name);
		glUniform3fv(loc, dataSize, (float *)vectors.data());
	}

}
