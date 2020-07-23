#pragma once

#include "lazy.hpp"
#include <memory>
#include <unordered_map>
#include "engine/renderer/Shader.hpp"

class ShaderManager
{
public:
	static ShaderManager &Get() {
		static ShaderManager manager;
		return manager;
	}
	ShaderManager(ShaderManager const &) = delete;
	void operator=(ShaderManager const &) = delete;

	std::tuple<unsigned int, opfor::Shader&> Create()
	{
		auto shader = opfor::Shader::Create();
		auto shaderId = _nextId++;

		_shaders[shaderId] = std::move(shader);

		return { shaderId, *_shaders[shaderId] };
	}

	std::optional<opfor::Shader*> Get(unsigned int id)
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
	std::unordered_map<unsigned int, std::unique_ptr<opfor::Shader>> _shaders;

	unsigned int _nextId = 0;

private:
	ShaderManager()
	{
	};
};
