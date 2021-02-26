#pragma once

#include "Shader.hpp"

namespace opfor {

class ShaderInstance
{
private:
	SharedPtr<Shader> _shader;

public:
	explicit ShaderInstance(SharedPtr<Shader> shader)
	{
		_shader = std::move(shader);
	}
};

}
