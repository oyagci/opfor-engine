project "RefurekuTests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "ExampleClass.cpp",
        "ExampleClass2.cpp",
        "CustomString.cpp",
        "ThirdPartyEnumReflectionCode.cpp",
        "Test.cpp",

        "RefurekuTests.cpp",
    }

    includedirs {
        "Include",
        "../Include",
        "../opfor-editor"
    }

    links {
        "RefurekuLibrary"
    }

    prebuildcommands {
        ("%{wks.location}/bin/" .. outputdir .. "/RefurekuGenerator/RefurekuGenerator.exe")
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"