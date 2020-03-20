#pragma once

#include <map>
#include <string>
#include <memory>
#include "Texture.hpp"

class TextureManager
{
public:
	static TextureManager &instance() {
		static TextureManager manager;
		return manager;
	}
	TextureManager(TextureManager const &) = delete;
	void operator=(TextureManager const &) = delete;

	void createTexture(std::string const &name, std::string const &path,
		std::vector<std::pair<GLenum, GLenum>>);
	void bind(std::string const &name, GLuint textureNumber);

private:
	std::map<std::string, std::unique_ptr<Texture>> _textures;

private:
	TextureManager() {};
};
