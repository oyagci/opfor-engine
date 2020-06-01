#pragma once

#include <string>
#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "lazy.hpp"
#include "engine/Mesh.hpp"

namespace assimp {

class Model
{
public:
	Model() = delete;
	Model(std::string const &path);

	std::vector<engine::Mesh> &getMeshes() {
		return _meshes;
	}

	struct Texture {
		std::string name;
		std::string path;
	};
	using TextureContainer = std::unordered_map<aiTextureType, std::vector<Texture>>;

	TextureContainer const &getTextures() const { return _textures; }

private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	engine::Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::string _directory;

	std::vector<std::string> loadMaterialTextures(aiMaterial *material, aiTextureType aitype);

	std::vector<engine::Mesh> _meshes;
	TextureContainer _textures;
};

}
