#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include "Texture.hpp"

class TextureManager
{
public:
	static TextureManager &Get() {
		static TextureManager manager;
		return manager;
	}
	TextureManager(TextureManager const &) = delete;
	void operator=(TextureManager const &) = delete;

	void createTexture(std::string const &name, std::string const &path,
		std::vector<std::pair<GLenum, GLenum>>, GLenum target, bool srgb = false);
	void bind(std::string const &name, GLuint textureNumber);
	void add(std::string const &name, Texture t);
	GLuint get(std::string const &name) { return _textures[name]->id(); }

private:
	std::map<std::string, std::unique_ptr<Texture>> _textures;

private:
	TextureManager() {};
};
