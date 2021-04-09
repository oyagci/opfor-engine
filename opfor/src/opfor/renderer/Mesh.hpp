#pragma once

#include "IDrawable.hpp"
#include "Material.hpp"
#include <opfor/renderer/Buffer.hpp>
#include <opfor/renderer/VertexArray.hpp>
#include <opfor/core/types/Vec3.hpp>
#include <opfor/core/types/Vec2.hpp>
#include <list>
#include <renderer.hpp>
#include <vector>

namespace opfor
{

struct MeshVertexData
{
    Array<float, 3> Position;
    Array<float, 3> Normal;
    Array<float, 2> Uv;
    Array<float, 4> Tangent;
};

class Mesh : public IDrawable
{
  public:
    enum class TextureType
    {
        TT_Diffuse,
        TT_Specular,
        TT_Normal,
    };

    struct Texture
    {
        String name;
        TextureType type;
    };

  private:
    List<Array<float, 3>> vPositions;
    List<Array<float, 3>> vNormals;
    List<Array<float, 2>> vUvs;
    List<Array<float, 4>> vTangents;
    Vector<uint32_t> indices;
    Vector<Texture> textures;

    SharedPtr<VertexArray> _vertexArray;

    String _material;
    std::optional<String> _pbrMaterial;

    void Move(Mesh &&other);

  public:
    Mesh();
    ~Mesh() = default;
    Mesh(Mesh &&);
    Mesh &operator=(Mesh &&);

    Mesh &addPosition(const Vec3 &v);
    Mesh &addNormal(const Vec3 &v);
    Mesh &addUv(const Vec2 &v);
    Mesh &addTangent(const glm::vec4 &v);
    Mesh &addIndex(const int index);
    Mesh &addTriangle(const glm::uvec3 &triangle);
    Mesh &addTexture(String const &name, TextureType type);

    auto const &GetPositions() const
    {
        return vPositions;
    }
    auto const &GetNormals() const
    {
        return vNormals;
    }
    auto const &GetUVs() const
    {
        return vUvs;
    }
    auto const &GetTangents() const
    {
        return vTangents;
    }
    auto const &GetIndices() const
    {
        return indices;
    }
    auto GetTextureIDs() const;
    auto const &GetVertexArray() const
    {
        return _vertexArray;
    }

    void SetMaterial(String name)
    {
        _material = name;
    }
    String GetMaterial() const
    {
        return _material;
    }

    void SetPbrMaterial(String name)
    {
        _pbrMaterial = name;
    }
    std::optional<String> GetPbrMaterial()
    {
        return _pbrMaterial;
    }

    Mesh &build();

    void Draw() const override;
    Vector<Texture> const &getTextures() const;
};

} // namespace opfor
