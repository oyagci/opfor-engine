#include "Mesh.hpp"
#include "TextureManager.hpp"
#include <opfor/renderer/Buffer.hpp>
#include <renderer.hpp>

namespace opfor
{

Mesh::Mesh()
{
    addTexture("prototype_tile_8", TextureType::TT_Diffuse);
}

Mesh::Mesh(Mesh &&other)
{
    Move(std::forward<Mesh>(other));
}

Mesh &Mesh::operator=(Mesh &&rhs)
{
    if (this != &rhs)
    {
        Move(std::forward<Mesh>(rhs));
    }
    return *this;
}

void Mesh::Move(Mesh &&other)
{
    vPositions = std::move(other.vPositions);
    vNormals = std::move(other.vNormals);
    vUvs = std::move(other.vUvs);
    vTangents = std::move(other.vTangents);
    indices = std::move(other.indices);
    textures = std::move(other.textures);
    _material = std::move(other._material);

    _vertexArray = std::move(other._vertexArray);

    _pbrMaterial = other._pbrMaterial;
    other._pbrMaterial.reset();
}

Mesh &Mesh::addPosition(const Vec3 &v)
{
    vPositions.push_back({v.x, v.y, v.z});

    return *this;
}

Mesh &Mesh::addNormal(const Vec3 &v)
{
    vNormals.push_back({v.x, v.y, v.z});

    return *this;
}

Mesh &Mesh::addUv(const Vec2 &v)
{
    vUvs.push_back({v.x, v.y});

    return *this;
}

Mesh &Mesh::addTangent(const glm::vec4 &v)
{
    vTangents.push_back({v.x, v.y, v.z, v.w});

    return *this;
}

Mesh &Mesh::addIndex(const int index)
{
    indices.push_back(index);

    return *this;
}

Mesh &Mesh::addTriangle(const glm::uvec3 &triangle)
{
    indices.push_back(triangle.x);
    indices.push_back(triangle.y);
    indices.push_back(triangle.z);

    return *this;
}

Mesh &Mesh::addTexture(String const &name, TextureType type)
{
    textures.push_back(Texture{name, type});

    return *this;
}

Vector<Mesh::Texture> const &Mesh::getTextures() const
{
    return textures;
}

Mesh &Mesh::build()
{
    _vertexArray = VertexArray::Create();
    _vertexArray->Bind();

    if (vPositions.size() > vNormals.size())
        vNormals.resize(vPositions.size(), {0.0f, 0.0f, 1.0f});
    if (vPositions.size() > vUvs.size())
        vUvs.resize(vPositions.size());
    if (vPositions.size() > vTangents.size())
        vTangents.resize(vPositions.size());

    OP4_CORE_ASSERT(vPositions.size() == vNormals.size() && vPositions.size() == vUvs.size() &&
                        vPositions.size() == vTangents.size(),
                    "Vertex Data is incomplete\n");

    Vector<MeshVertexData> vertices;

    const size_t count = vPositions.size();
    vertices.reserve(count);

    for (size_t i = 0; i < count; i++)
    {

        auto const &position = vPositions.front();
        auto const &normal = vNormals.front();
        auto const &tex_coord = vUvs.front();
        auto const &tangent = vTangents.front();

        MeshVertexData vertexData = {position, normal, tex_coord, tangent};

        vertices.push_back(vertexData);

        vPositions.pop_front();
        vNormals.pop_front();
        vUvs.pop_front();
        vTangents.pop_front();
    }

    auto vertexBuffer =
        VertexBuffer::Create(reinterpret_cast<float *>(vertices.data()), sizeof(MeshVertexData) * vertices.size());
    auto indexBuffer = IndexBuffer::Create(reinterpret_cast<uint32_t *>(indices.data()), indices.size());

    BufferLayout layout = {
        {ShaderDataType::Float3, "in_position"},
        {ShaderDataType::Float3, "in_normal"},
        {ShaderDataType::Float2, "tex_coords"},
        {ShaderDataType::Float4, "in_tangent"},
    };
    vertexBuffer->SetLayout(layout);

    _vertexArray->AddVertexBuffer(std::move(vertexBuffer));
    _vertexArray->SetIndexBuffer(std::move(indexBuffer));

    _vertexArray->Unbind();

    return *this;
}

void Mesh::Draw() const
{
    _vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_vertexArray->GetIndexBuffer()->GetCount()), GL_UNSIGNED_INT,
                   nullptr);
    _vertexArray->Unbind();
}

auto Mesh::GetTextureIDs() const
{
    Vector<GLuint> ids(textures.size());

    for (auto const &t : textures)
    {
        GLuint id = TextureManager::Get().Get(t.name)->GetRawHandle();
        ids.push_back(id);
    }

    return ids;
}
} // namespace opfor
