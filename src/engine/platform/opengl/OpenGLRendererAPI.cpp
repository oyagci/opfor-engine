#include "OpenGLRendererAPI.hpp"
#include "engine/renderer/VertexArray.hpp"
#include "engine/renderer/Framebuffer.hpp"
#include "engine/renderer/Texture.hpp"
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

		glActiveTexture((GLenum)unit);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
		_prevTextureUnits[unit].push_back(prevTexture);

		glBindTexture(GL_TEXTURE_2D, texture->GetRawHandle());
	}

	void OpenGLRendererAPI::PopTexture(TextureUnit unit)
	{
		auto prevTexture = _prevTextureUnits[unit].back();

		glActiveTexture((GLenum)unit);
		glBindTexture(GL_TEXTURE_2D, prevTexture);

		_prevTextureUnits[unit].pop_back();
	}

}
