#pragma once

#include "lazy.hpp"

namespace assimp {

class Mesh : public lazy::graphics::Mesh
{
private:
	std::vector<std::string> _textures;

public:
	Mesh() : lazy::graphics::Mesh()
	{
		addTexture("prototype_tile_8");
	}

	void addTexture(std::string const &name)
	{
		_textures.push_back(name);
	}

	std::vector<std::string> const &getTextures()
	{
		return _textures;
	}
};

}
