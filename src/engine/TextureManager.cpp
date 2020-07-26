#include "TextureManager.hpp"
#include <fmt/format.h>

opfor::SharedPtr<opfor::Texture> TextureManager::Create(std::string const &name)
{
	if (_textures.find(name) != _textures.end()) {
		OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
	}

	opfor::SharedPtr<opfor::Texture> texture = opfor::Texture::Create();
	_textures[name] = texture;

	return texture;
}

void TextureManager::Add(std::string const &name, opfor::SharedPtr<opfor::Texture> texture)
{
	if (_textures.find(name) != _textures.end()) {
		OP4_CORE_WARNING("Texture named \"{}\" already exists!\n", name);
	}

	_textures[name] = texture;
}
