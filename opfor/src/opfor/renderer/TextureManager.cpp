#include "TextureManager.hpp"
#include <fmt/format.h>

namespace opfor
{

SharedPtr<Texture2D> TextureManager::Create2D(String const &name)
{
    if (_textures.find(name) != _textures.end())
    {
        OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
    }

    SharedPtr<Texture> texture = Texture2D::Create();
    _textures[name] = texture;

    return std::static_pointer_cast<Texture2D>(texture);
}

void TextureManager::Add(String const &name, SharedPtr<Texture> texture)
{
    if (_textures.find(name) != _textures.end())
    {
        OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
    }

    _textures[name] = texture;
}

} // namespace opfor
