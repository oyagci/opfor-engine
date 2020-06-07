#pragma once

#include <string>
#include <fmt/format.h>

struct PbrMaterial
{
	std::string Name;
	std::optional<std::string> Albedo;
	std::optional<std::string> Normal;
	std::optional<std::string> Metallic;
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
