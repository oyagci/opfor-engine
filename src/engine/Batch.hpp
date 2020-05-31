#pragma once
#include <vector>
#include "lazy.hpp"

namespace engine {

class Mesh;

///
/// Group together (batch) multiple meshes that can be rendered in a single drawcall
///
class Batch
{
private:
	struct BatchMesh {
		std::vector<GLfloat> positions;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> uvs;
		std::vector<GLfloat> tangents;
		std::vector<GLfloat> materials;
		std::vector<GLuint>  indices;
	};

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	BatchMesh _batchMesh;

	// On OpenGL 4, the minimun number of texture units must be at least 80
	static constexpr GLuint MaxNumberOfTextureUnits = 80;

public:
	Batch();

	/// Add a mesh to the batch
	void AddMesh(Mesh const &m);

	/// After this, the batch can be drawn with a call to Draw()
	void Build();

	/// Draw the batch
	void Draw();
};

}
