#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <string>

namespace opfor
{

class Model
{
  private:
    unsigned int _id = 0;
    std::vector<unsigned int> _meshes;

    std::optional<std::vector<unsigned int>> TinyLoader(std::string const &path);

  public:
    Model() = default;

    bool LoadFromGLTF(std::string const &path)
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
