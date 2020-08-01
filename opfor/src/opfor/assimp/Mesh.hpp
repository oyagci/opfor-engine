#pragma once

#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace assimp {

struct Mesh
{
	std::vector<float> Positions;
	std::vector<float> Normals;
	std::vector<float> Tangents;
	std::vector<float> TexCoords;
	std::vector<unsigned int> Indices;
	std::vector<std::string> DiffuseMaps;
	std::vector<std::string> NormalMaps;
	std::vector<std::string> SpecularMaps;
};

}
