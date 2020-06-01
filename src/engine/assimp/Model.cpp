#include "Model.hpp"
#include <iostream>
#include <vector>
#include <fmt/format.h>

namespace assimp {

Model::Model(std::string const &path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fmt::print("Could not load model");
		return ;
	}
	_directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

engine::Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	engine::Mesh engineMesh;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		// Vertex Position
		auto vert = mesh->mVertices[i];
		engineMesh.addPosition(glm::vec3(vert.x, vert.y, vert.z));

		// Vertex Normal
		auto norm = mesh->mNormals[i];
		engineMesh.addNormal(glm::vec3(norm.x, norm.y, norm.z));

		// Vertex Tangent
		auto tangent = mesh->mTangents[i];
		engineMesh.addTangent(glm::vec3(tangent.x, tangent.y, tangent.z));

		if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
			glm::vec2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			engineMesh.addUv(tex);
		}
	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto &face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			engineMesh.addIndex(face.mIndices[j]);
		}
	}

	// Textures
	if (mesh->mMaterialIndex > 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		auto diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		for (auto &t : diffuse) {
			engineMesh.addTexture(t, engine::Mesh::TextureType::TT_Diffuse);
		}
		auto specular = loadMaterialTextures(material, aiTextureType_SPECULAR);
		for (auto &s : specular) {
			engineMesh.addTexture(s, engine::Mesh::TextureType::TT_Specular);
		}
		auto normal = loadMaterialTextures(material, aiTextureType_HEIGHT);
		for (auto &n : normal) {
			engineMesh.addTexture(n, engine::Mesh::TextureType::TT_Normal);
		}
	}

	engineMesh.build();

	return engineMesh;
}

std::vector<std::string> Model::loadMaterialTextures(aiMaterial *material, aiTextureType aitype)
{
	std::vector<std::string> textureNames;

	for (unsigned int i = 0; i < material->GetTextureCount(aitype); i++) {
		aiString str;
		material->GetTexture(aitype, i, &str);
		std::string path = _directory + "/" + str.C_Str();
		textureNames.push_back(str.C_Str());

		_textures[aitype].push_back({ str.C_Str(), path });
	}

	return textureNames;
}

}
