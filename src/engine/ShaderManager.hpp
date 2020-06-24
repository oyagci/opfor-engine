#pragma once

#include "lazy.hpp"
#include <memory>
#include <unordered_map>

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
		auto shaderId = _nextId++;

		_shaders[shaderId] = std::move(shader);

		return { shaderId, *_shaders[shaderId] };
	}

	std::optional<lazy::graphics::Shader*> Get(unsigned int id)
	{
		if (_shaders.find(id) != _shaders.end()) {
			return std::make_optional(_shaders[id].get());
		}

		return std::nullopt;
	}

	void Delete(unsigned int id)
	{
		if (_shaders.find(id) == _shaders.end()) { return ; }

		_shaders.erase(id);
	}

private:
	std::unordered_map<unsigned int, std::unique_ptr<lazy::graphics::Shader>> _shaders;

	unsigned int _nextId = 0;

private:
	ShaderManager()
	{
	};
};
