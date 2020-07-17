#pragma once
#include <vector>
#include "lazy.hpp"
#include "IDrawable.hpp"

namespace opfor {

///
/// Group together (batch) multiple meshes that can be rendered in a single drawcall
///
class Batch : public IDrawable
{
private:
	std::vector<GLfloat> _positions;
	std::vector<GLfloat> _normals;
	std::vector<GLfloat> _uvs;
	std::vector<GLfloat> _tangents;
	std::vector<std::string> _materials;
	std::vector<GLuint> _indices;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	// On OpenGL 4, the minimun number of texture units must be at least 80
	static constexpr GLuint MaxNumberOfTextureUnits = 80;

public:
	Batch();

	/// Add a mesh to the batch
	void AddMesh(
		std::vector<GLfloat> const &positions,
		std::vector<GLfloat> const &normals,
		std::vector<GLfloat> const &uvs,
		std::vector<GLfloat> const &tangents,
		std::string const &material,
		std::vector<GLuint> const &indices
	);

	/// After this, the batch can be drawn with a call to Draw()
	void Build();

	/// Draw the batch
	void Draw() const override;
};

}
