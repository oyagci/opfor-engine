#pragma once

#include "lazy.hpp"

namespace assimp {

enum class TextureType {
	TT_Diffuse,
	TT_Specular,
	TT_Normal,
};

struct Texture {
	std::string name;
	TextureType type;
};

class Mesh : public lazy::graphics::Mesh
{
private:
	std::vector<Texture> _textures;

public:
	Mesh() : lazy::graphics::Mesh()
	{
		addTexture("prototype_tile_8", TextureType::TT_Diffuse);
	}

	void addTexture(std::string const &name, TextureType type)
	{
		_textures.push_back(Texture { name, type });
	}

	std::vector<Texture> const &getTextures()
	{
		return _textures;
	}
};

}
