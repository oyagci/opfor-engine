#pragma once

#include "opfor/core/base.hpp"
#include <string>
#include <fmt/format.h>
#include <optional>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Shader.hpp"

struct PbrMaterial
{
	std::string Name;
	glm::vec4 BaseColor{};
	std::optional<std::string> Albedo;
	std::optional<std::string> Normal;
	std::optional<std::string> MetallicRoughness;

	float RoughnessFactor;
	float MetallicFactor;

	bool hasMetallic;
	bool hasRoughness;
};

/// Keeps track of which OpenGL texture id each material uses
struct Material
{
	std::string name;
	unsigned int diffuse;
	unsigned int normal;
	unsigned int specular;
	float shininess;
};

template<>
struct fmt::formatter<Material>
{
	constexpr auto parse(format_parse_context &ctx)
	{
		auto it = ctx.begin();
		auto end = ctx.end();

		if (it != end && *it != '}') {
			throw format_error("invalid format");
		}

		return it;
	}

	template <typename FormatContext>
	auto format(const Material &m, FormatContext &ctx)
	{
		return format_to(ctx.out(),
			"{{ diffuse: {: >2}, normal: {: >2}, specular: {: >2}, shininess: {: >3} }}\n",
			m.diffuse, m.normal, m.specular, m.shininess);
	}
};

template<>
struct fmt::formatter<PbrMaterial>
{
	constexpr auto parse(format_parse_context &ctx)
	{
		auto it = ctx.begin();
		auto end = ctx.end();

		if (it != end && *it != '}') {
			throw format_error("invalid format");
		}

		return it;
	}

	template <typename FormatContext>
	auto format(const PbrMaterial &m, FormatContext &ctx)
	{
		return format_to(ctx.out(),
			"Material \"{}\" {{\n"
			"  BaseColor: {{ {}, {}, {}, {} }},\n"
			"  Albedo: {},\n"
			"  Normal: {},\n"
			"  MetallicRoughness: {},\n"
			"  MetallicFactor: {},\n"
			"  RoughnessFactor: {},\n"
			"}}",
			m.Name,
			m.BaseColor[0], m.BaseColor[1], m.BaseColor[2], m.BaseColor[3],
			m.Albedo.value_or("(null)"),
			m.Normal.value_or("(null)"),
			m.MetallicRoughness.value_or("(null)"),
			m.MetallicFactor,
			m.RoughnessFactor
		);
	}
};

namespace opfor
{

class AMaterial
{
public:
	enum class UniformType
	{
		Int,
		UInt,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4,
	};

	union UniformValue
	{
		int Int;
		unsigned int UInt;
		float Float;
		glm::vec2 Vec2;
		glm::vec3 Vec3;
		glm::vec4 Vec4;
		glm::mat3 Mat3;
		glm::mat4 Mat4;
	};

	struct Uniform
	{
		UniformType Type;
		UniformValue Value;
	};

private:
	UnorderedMap<String, Uniform> _uniforms{};

	Optional<Shader *> _shader = nullptr;

public:
	virtual ~AMaterial() = default;

	[[nodiscard]] const UnorderedMap<String, Uniform> &GetAllUniforms() const { return _uniforms; }
	[[nodiscard]] Uniform GetUniform(const String &name) const { return _uniforms.at(name); }

	[[nodiscard]] Optional<Shader *> GetShader() const { return _shader; }
	void SetShader(Shader *shader) { _shader = shader; }

	void SetUniform(const String &name, int value)
	{
		Uniform u{};
		u.Type = UniformType::Int;
		u.Value.Int = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, unsigned int value)
	{
		Uniform u{};
		u.Type = UniformType::UInt;
		u.Value.UInt = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, float value)
	{
		Uniform u{};
		u.Type = UniformType::Float;
		u.Value.Float = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, glm::vec2 value)
	{
		Uniform u{};
		u.Type = UniformType::Vec2;
		u.Value.Vec2 = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, glm::vec3 value)
	{
		Uniform u{};
		u.Type = UniformType::Vec3;
		u.Value.Vec3 = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, glm::vec4 value)
	{
		Uniform u{};
		u.Type = UniformType::Vec4;
		u.Value.Vec4 = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, glm::mat3 value)
	{
		Uniform u{};
		u.Type = UniformType::Mat3;
		u.Value.Mat3 = value;
		_uniforms[name] = u;
	}

	void SetUniform(const String &name, glm::mat4 value)
	{
		Uniform u{};
		u.Type = UniformType::Mat4;
		u.Value.Mat4 = value;
		_uniforms[name] = u;
	}
};

}
