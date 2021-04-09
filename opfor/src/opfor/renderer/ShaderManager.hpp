#pragma once

#include "opfor/renderer/Shader.hpp"
#include "renderer.hpp"
#include <memory>
#include <unordered_map>

namespace opfor
{

class ShaderManager
{
  public:
    static ShaderManager &Get()
    {
        static ShaderManager manager;
        return manager;
    }
    ShaderManager(ShaderManager const &) = delete;
    void operator=(ShaderManager const &) = delete;

    std::tuple<unsigned int, Shader &> Create(std::string const &path)
    {
        auto shader = Shader::Create(path);
        auto shaderId = _nextId++;

        _shaders[shaderId] = std::move(shader);

        return {shaderId, *_shaders[shaderId]};
    }

    Optional<SharedPtr<Shader>> Get(unsigned int id)
    {
        if (_shaders.find(id) != _shaders.end())
        {
            return std::make_optional(_shaders[id]);
        }

        return std::nullopt;
    }

    void Delete(unsigned int id)
    {
        if (_shaders.find(id) == _shaders.end())
        {
            return;
        }

        _shaders.erase(id);
    }

  private:
    UnorderedMap<unsigned int, SharedPtr<Shader>> _shaders;

    unsigned int _nextId = 0;

  private:
    ShaderManager() = default;
};

} // namespace opfor
