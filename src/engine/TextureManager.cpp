#include "TextureManager.hpp"
#include <fmt/format.h>

void TextureManager::createTexture(std::string const &name, std::string const &path,
	std::vector<std::pair<GLenum, GLenum>> parameters, GLenum target, bool srgb)
{
	if (_textures.find(name) != _textures.end()) { return ; }

	_textures[name] = std::make_unique<Texture>(name, target);

	_textures[name]->bind(0);
	for (auto const &p : parameters) {
		_textures[name]->setParameter(GL_TEXTURE_2D, p.first, p.second);
	}
	_textures[name]->setSRGB(srgb);
	_textures[name]->load(path);
}

void TextureManager::bind(std::string const &name, GLuint textureNumber)
{
	if (_textures.find(name) == _textures.end()) {
		throw std::runtime_error("Texture does not exist");
	}
	_textures[name]->bind(textureNumber);
}

void TextureManager::add(std::string const &name, Texture t)
{
	auto pTexture = std::make_unique<Texture>(std::move(t));
	_textures[name] = std::move(pTexture);
}
