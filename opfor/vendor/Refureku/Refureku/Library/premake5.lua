project "RefurekuLibrary"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/Exceptions/ArgCountMismatch.cpp",
        "Source/Exceptions/TypeMismatch.cpp",
        "Source/Exceptions/ReturnTypeMismatch.cpp",
        "Source/Exceptions/ArgTypeMismatch.cpp",
        "Source/Exceptions/BadNamespaceFormat.cpp",
        "Source/Exceptions/OutOfRange.cpp",

        "Source/TypeInfo/TypePart.cpp",
        "Source/TypeInfo/Type.cpp",
        "Source/TypeInfo/EnumValue.cpp",
        "Source/TypeInfo/Database.cpp",

        "Source/TypeInfo/Entity/Entity.cpp",
        "Source/TypeInfo/Entity/EntityCast.cpp",
        "Source/TypeInfo/Entity/DefaultEntityRegisterer.cpp",

        "Source/TypeInfo/Namespaces/Namespace.cpp",
        "Source/TypeInfo/Namespaces/NamespaceFragment.cpp",
        "Source/TypeInfo/Namespaces/NamespaceFragmentRegisterer.cpp",

        "Source/TypeInfo/Archetypes/Archetype.cpp",
        "Source/TypeInfo/Archetypes/FundamentalArchetype.cpp",
        "Source/TypeInfo/Archetypes/Enum.cpp",
        "Source/TypeInfo/Archetypes/Struct.cpp",
        "Source/TypeInfo/Archetypes/Class.cpp",
        "Source/TypeInfo/Archetypes/ArchetypeRegisterer.cpp",
        "Source/TypeInfo/Archetypes/GetArchetype.cpp",

        "Source/TypeInfo/Variables/VariableBase.cpp",
        "Source/TypeInfo/Variables/Variable.cpp",
        "Source/TypeInfo/Variables/FieldBase.cpp",
        "Source/TypeInfo/Variables/Field.cpp",
        "Source/TypeInfo/Variables/StaticField.cpp",

        "Source/TypeInfo/Functions/FunctionBase.cpp",
        "Source/TypeInfo/Functions/Function.cpp",
        "Source/TypeInfo/Functions/MethodBase.cpp",
        "Source/TypeInfo/Functions/Method.cpp",
        "Source/TypeInfo/Functions/StaticMethod.cpp",
        "Source/TypeInfo/Functions/FunctionParameter.cpp",

        "Source/TypeInfo/Properties/Property.cpp",
    }

    includedirs {
        "Include",
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"