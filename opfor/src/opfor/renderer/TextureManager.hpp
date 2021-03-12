#pragma once

#include "opfor/renderer/Texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class TextureManager
{
  public:
    static TextureManager &Get()
    {
        static TextureManager manager;
        return manager;
    }
    TextureManager(TextureManager const &) = delete;
    void operator=(TextureManager const &) = delete;

    opfor::SharedPtr<opfor::Texture2D> Create2D(std::string const &name);
    void Add(std::string const &name, opfor::SharedPtr<opfor::Texture> texture);
    auto Get(std::string const &name)
    {
        return _textures[name];
    }

  private:
    std::unordered_map<std::string, opfor::SharedPtr<opfor::Texture>> _textures;

  private:
    TextureManager() = default;
};
