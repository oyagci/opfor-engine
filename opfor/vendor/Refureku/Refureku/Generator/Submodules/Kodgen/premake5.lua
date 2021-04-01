project "Kodgen"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    runtime "Release"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/Properties/PropertyParsingSettings.cpp",
        "Source/Properties/Property.cpp",
        "Source/Properties/SimpleProperty.cpp",
        "Source/Properties/ComplexProperty.cpp",
        "Source/Properties/PropertyGroup.cpp",
        "Source/Properties/SimplePropertyRule.cpp",
        "Source/Properties/ComplexPropertyRule.cpp",
        "Source/Properties/DefaultSimplePropertyRule.cpp",
        "Source/Properties/DefaultComplexPropertyRule.cpp",
        "Source/Properties/ComplexPropertySubProperty.cpp",
        "Source/Properties/FixedComplexPropertyRule.cpp",

        "Source/Properties/NativeProperties/ParseAllNestedPropertyRule.cpp",
        "Source/Properties/NativeProperties/GenCodeTemplatePropertyRule.cpp",

        "Source/InfoStructures/EntityInfo.cpp",
        "Source/InfoStructures/NamespaceInfo.cpp",
        "Source/InfoStructures/VariableInfo.cpp",
        "Source/InfoStructures/FieldInfo.cpp",
        "Source/InfoStructures/FunctionInfo.cpp",
        "Source/InfoStructures/MethodInfo.cpp",
        "Source/InfoStructures/StructClassInfo.cpp",
        "Source/InfoStructures/NestedStructClassInfo.cpp",
        "Source/InfoStructures/EnumInfo.cpp",
        "Source/InfoStructures/NestedEnumInfo.cpp",
        "Source/InfoStructures/EnumValueInfo.cpp",
        "Source/InfoStructures/TypeInfo.cpp",

        "Source/Parsing/ParsingError.cpp",
        "Source/Parsing/PropertyParser.cpp",
        "Source/Parsing/EntityParser.cpp",
        "Source/Parsing/NamespaceParser.cpp",
        "Source/Parsing/ClassParser.cpp",
        "Source/Parsing/VariableParser.cpp",
        "Source/Parsing/FieldParser.cpp",
        "Source/Parsing/FunctionParser.cpp",
        "Source/Parsing/MethodParser.cpp",
        "Source/Parsing/EnumParser.cpp",
        "Source/Parsing/EnumValueParser.cpp",
        "Source/Parsing/FileParser.cpp",
        "Source/Parsing/FileParserFactoryBase.cpp",
        "Source/Parsing/ParsingSettings.cpp",

        "Source/Misc/EAccessSpecifier.cpp",
        "Source/Misc/Helpers.cpp",
        "Source/Misc/DefaultLogger.cpp",
        "Source/Misc/CompilerHelpers.cpp",
        "Source/Misc/System.cpp",
        "Source/Misc/Filesystem.cpp",
        "Source/Misc/TomlUtility.cpp",

        "Source/CodeGen/FileGenerationUnit.cpp",
        "Source/CodeGen/FileGenerationResult.cpp",
        "Source/CodeGen/FileGenerationError.cpp",
        "Source/CodeGen/FileGenerator.cpp",
        "Source/CodeGen/FileGenerationSettings.cpp",
        "Source/CodeGen/GeneratedFile.cpp",

        "Source/Threading/ThreadPool.cpp",
        "Source/Threading/TaskBase.cpp",
    }

    includedirs {
        "Include",
        "ThirdParty/Include",
    }

    libdirs {
        "ThirdParty/x64/Static",
        "ThirdParty/x64/Shared",
    }

    links {
        "clang"
    }

    filter "system:windows"
        -- Copy libclang shared library & vswhere to the bin folder

        postbuildcommands {
            '{COPYFILE} "ThirdParty/x64/Shared/libclang.dll" "%{wks.location}bin/' .. outputdir .. '/%{prj.name}/libclang.dll"',
            '{COPYFILE} "ThirdParty/x64/Bin/vswhere.exe" "%{wks.location}/bin/' .. outputdir .. '/%{prj.name}/vswhere.exe"',
        }

    filter "system:linux"
        -- Copy libclang shared library to the bin folder

        postbuildcommands {
            '{COPYFILE} "ThirdParty/x64/Shared/libclang.so.10" "%{wks.location}/bin/' .. outputdir .. '/%{prj.name}/libclang.so.10"',
            'ln -sf "libclang.so.10" "%{wks.location}/bin/' .. outputdir .. '/%{prj.name}/libclang.so"',
        }

		linkoptions { "-L %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/ -lclang"}

