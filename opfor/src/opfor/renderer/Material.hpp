#pragma once

#include <opfor/core/base.hpp>
#include <opfor/core/types/Vec4.hpp>
#include <fmt/format.h>
#include <optional>
#include <string>

struct PbrMaterial
{
    opfor::String Name;
    opfor::Vec4 BaseColor{};
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

template <> struct fmt::formatter<Material>
{
    constexpr auto parse(format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it != end && *it != '}')
        {
            throw format_error("invalid format");
        }

        return it;
    }

    template <typename FormatContext> auto format(const Material &m, FormatContext &ctx)
    {
        return format_to(ctx.out(), "{{ diffuse: {: >2}, normal: {: >2}, specular: {: >2}, shininess: {: >3} }}\n",
                         m.diffuse, m.normal, m.specular, m.shininess);
    }
};

template <> struct fmt::formatter<PbrMaterial>
{
    constexpr auto parse(format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it != end && *it != '}')
        {
            throw format_error("invalid format");
        }

        return it;
    }

    template <typename FormatContext> auto format(const PbrMaterial &m, FormatContext &ctx)
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
                         m.Name, m.BaseColor[0], m.BaseColor[1], m.BaseColor[2], m.BaseColor[3],
                         m.Albedo.value_or("(null)"), m.Normal.value_or("(null)"),
                         m.MetallicRoughness.value_or("(null)"), m.MetallicFactor, m.RoughnessFactor);
    }
};
