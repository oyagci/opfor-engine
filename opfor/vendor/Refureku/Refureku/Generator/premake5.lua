include "Submodules/Kodgen"

project "RefurekuGeneratorLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    runtime "Release"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/Properties/DefaultSimplePropertyRule.cpp",
        "Source/Properties/DefaultComplexPropertyRule.cpp",
        "Source/Properties/CustomInstantiatorPropertyRule.cpp",
        "Source/Properties/PropertySettingsPropertyRule.cpp",
        "Source/Properties/TestPropertyRule.cpp",

        "Source/Properties/CodeGenData/PropertyCodeGenData.cpp",
        "Source/Properties/CodeGenData/PropertyCodeGenFileHeaderData.cpp",
        "Source/Properties/CodeGenData/PropertyCodeGenPropertyAddData.cpp",
        "Source/Properties/CodeGenData/PropertyCodeGenClassFooterData.cpp",
        "Source/Properties/CodeGenData/PropertyCodeGenFileFooterData.cpp",

        "Source/Parsing/FileParser.cpp",
        
        "Source/CodeGen/GeneratedEntityCodeTemplate.cpp",
        "Source/CodeGen/GeneratedNamespaceCodeTemplate.cpp",
        "Source/CodeGen/GeneratedClassCodeTemplate.cpp",
        "Source/CodeGen/GeneratedEnumCodeTemplate.cpp",
        "Source/CodeGen/GeneratedVariableCodeTemplate.cpp",
        "Source/CodeGen/GeneratedFunctionCodeTemplate.cpp",
        "Source/CodeGen/FileGenerationUnit.cpp",
        "Source/CodeGen/FileGenerator.cpp",
    }

    includedirs {
        "Include",
        "Submodules/Kodgen/Include",
        "Submodules/Kodgen/ThirdParty/Include",
    }

    links {
        "Kodgen"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"

project "RefurekuGenerator"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    runtime "Release"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/main.cpp"
    }

    includedirs {
        "Include",
        "Submodules/Kodgen/Include",
        "Submodules/Kodgen/ThirdParty/Include",
    }

    links {
        "Kodgen",
        "RefurekuGeneratorLib",
    }

    postbuildcommands {
        "{COPYFILE} " .. ("%{prj.location}/" .. "RefurekuSettings.toml") .. " " .. ("%{wks.location}" .. "/bin/" .. outputdir .. "/%{prj.name}/RefurekuSettings.toml")
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"

	filter "system:linux"
		links {
			"pthread",
			"Kodgen",
		}
		linkoptions { "-L %{wks.location}/bin/" .. outputdir .. "/Kodgen/ -lclang"}

    filter "system:windows"
        -- Copy libclang shared library & vswhere to the bin folder

        p1 = "%{prj.location}/Submodules/Kodgen/ThirdParty/x64/Shared/libclang.dll"
        p2 = "%{wks.location}bin/" .. outputdir .. "/%{prj.name}/libclang.dll"

        prebuildcommands {
            '{COPYFILE} %{p1} %{p2}',
        }
