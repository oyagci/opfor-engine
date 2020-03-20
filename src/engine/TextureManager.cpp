#include "TextureManager.hpp"

void TextureManager::createTexture(std::string const &name, std::string const &path,
	std::vector<std::pair<GLenum, GLenum>> parameters)
{
	if (_textures.find(name) != _textures.end()) { return ; }

	_textures[name] = std::make_unique<Texture>(name);

	_textures[name]->bind(GL_TEXTURE0);
	for (auto const &p : parameters) {
		_textures[name]->setParameter(GL_TEXTURE_2D, p.first, p.second);
	}
	_textures[name]->load(path);
}

void TextureManager::bind(std::string const &name, GLuint textureNumber)
{
	_textures[name]->bind(textureNumber);
}
