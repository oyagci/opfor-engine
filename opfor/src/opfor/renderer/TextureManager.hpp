#pragma once

#include <opfor/core/base.hpp>
#include <opfor/renderer/Texture.hpp>

namespace opfor
{

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

    SharedPtr<Texture2D> Create2D(String const &name);
    void Add(String const &name, SharedPtr<Texture> texture);
    auto Get(String const &name)
    {
        return _textures[name];
    }

  private:
    UnorderedMap<String, SharedPtr<Texture>> _textures;

  private:
    TextureManager() = default;
};

} // namespace opfor
