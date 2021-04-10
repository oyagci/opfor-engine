#pragma once

#include <opfor/core/base.hpp>

namespace opfor
{

class Model
{
  private:
    unsigned int _id = 0;
    Vector<unsigned int> _meshes;

    Optional<Vector<unsigned int>> TinyLoader(String const &path);

  public:
    Model() = default;

    bool LoadFromGLTF(String const &path)
    {
        auto const meshes = TinyLoader(path);

        if (meshes.has_value())
        {
            for (auto const mesh : meshes.value())
            {
                _meshes.push_back(mesh);
            }
        }

        return meshes.has_value();
    }

    auto const &GetMeshes()
    {
        return _meshes;
    }
};

} // namespace opfor
