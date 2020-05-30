#pragma once

#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "lazy.hpp"
#include "Texture.hpp"
#include "engine/Mesh.hpp"

namespace assimp {

class Model
{
private:

public:
	Model() = delete;
	Model(std::string const &path);

	std::vector<engine::Mesh> &getMeshes() {
		return _meshes;
	}

private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	engine::Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::string _directory;

	std::vector<std::string> loadMaterialTextures(aiMaterial *material, aiTextureType aitype,
		GLenum target);

	std::vector<engine::Mesh> _meshes;
};

}
