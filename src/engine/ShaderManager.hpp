#pragma once

#include "lazy.hpp"
#include <memory>
#include <vector>

class ShaderManager
{
public:
	static ShaderManager &instance() {
		static ShaderManager manager;
		return manager;
	}
	ShaderManager(ShaderManager const &) = delete;
	void operator=(ShaderManager const &) = delete;

	std::tuple<unsigned int, lazy::graphics::Shader&> Create()
	{
		auto shader = std::make_unique<lazy::graphics::Shader>();

		_shaders.push_back(std::move(shader));

		return { _shaders.size() - 1, *_shaders.back() };
	}
	std::optional<lazy::graphics::Shader*> Get(unsigned int id)
	{
		std::optional<lazy::graphics::Shader*> shader;
		if (id < _shaders.size()) {
			shader = _shaders[id].get();
		}
		return shader;
	}

private:
	std::vector<std::unique_ptr<lazy::graphics::Shader>> _shaders;

private:
	ShaderManager()
	{
		_shaders.reserve(10);
	};
};
