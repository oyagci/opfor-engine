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

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	Mesh meshVert;

	meshVert.Positions.reserve(mesh->mNumVertices * 3);
	meshVert.Normals.reserve(mesh->mNumVertices * 3);
	meshVert.Tangents.reserve(mesh->mNumVertices * 3);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		// Vertex Position
		auto vert = mesh->mVertices[i];
		meshVert.Positions.push_back(vert.x);
		meshVert.Positions.push_back(vert.y);
		meshVert.Positions.push_back(vert.z);

		// Vertex Normal
		auto norm = mesh->mNormals[i];
		meshVert.Normals.push_back(norm.x);
		meshVert.Normals.push_back(norm.y);
		meshVert.Normals.push_back(norm.z);

		// Vertex Tangent
		auto tangent = mesh->mTangents[i];
		meshVert.Tangents.push_back(tangent.x);
		meshVert.Tangents.push_back(tangent.y);
		meshVert.Tangents.push_back(tangent.z);

		if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
			glm::vec2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			meshVert.TexCoords.push_back(tex.x);
			meshVert.TexCoords.push_back(tex.y);
		}
	}

	// Indices
	meshVert.Indices.reserve(mesh->mNumFaces * 3);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto &face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			meshVert.Indices.push_back(face.mIndices[j]);
		}
	}

	// Textures
	if (mesh->mMaterialIndex > 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		auto diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		meshVert.DiffuseMaps.reserve(diffuse.size());
		for (auto &t : diffuse) {
			meshVert.DiffuseMaps.push_back(t);
		}

		auto specular = loadMaterialTextures(material, aiTextureType_SPECULAR);
		meshVert.SpecularMaps.reserve(specular.size());
		for (auto &s : specular) {
			meshVert.SpecularMaps.push_back(s);
		}

		auto normal = loadMaterialTextures(material, aiTextureType_HEIGHT);
		meshVert.NormalMaps.reserve(normal.size());
		for (auto &n : normal) {
			meshVert.NormalMaps.push_back(n);
		}
	}

	return meshVert;
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
