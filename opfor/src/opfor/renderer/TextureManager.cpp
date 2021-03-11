#include "TextureManager.hpp"
#include <fmt/format.h>

opfor::SharedPtr<opfor::Texture2D> TextureManager::Create2D(std::string const &name)
{
    if (_textures.find(name) != _textures.end())
    {
        OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
    }

    opfor::SharedPtr<opfor::Texture> texture = opfor::Texture2D::Create();
    _textures[name] = texture;

    return std::static_pointer_cast<opfor::Texture2D>(texture);
}

void TextureManager::Add(std::string const &name, opfor::SharedPtr<opfor::Texture> texture)
{
    if (_textures.find(name) != _textures.end())
    {
        OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
    }

    _textures[name] = texture;
}
