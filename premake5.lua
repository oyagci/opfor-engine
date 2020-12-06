workspace "opfor"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]     = "opfor/vendor/glfw/include"
IncludeDir["GLAD"]     = "opfor/vendor/glad/include"
IncludeDir["fmt"]      = "opfor/vendor/fmt/include"
IncludeDir["ImGui"]    = "opfor/vendor/imgui"
IncludeDir["tinygltf"] = "opfor/vendor/tinygltf"
IncludeDir["glm"]      = "opfor/vendor/glm"
IncludeDir["stduuid"]  = "opfor/vendor/stduuid/include"
IncludeDir["NFD"]      = "opfor/vendor/nativefiledialog/src/include"
IncludeDir["ImGuizmo"] = "opfor/vendor/ImGuizmo"

include "opfor/vendor/glfw"
include "opfor/vendor/glad"
include "opfor/vendor/fmt"
include "opfor/vendor/imgui"
include "opfor/vendor/ImGuizmo"
include "opfor/vendor/nativefiledialog"

project "opfor"
    location "opfor"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
    }

    includedirs
    {
	    "%{prj.name}/src",
        "%{prj.name}/src/opfor",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.tinygltf}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stduuid}",
		"%{IncludeDir.NFD}",
		"%{IncludeDir.ImGuizmo}",
    }

	links
	{
        "GLFW",
        "GLAD",
        "fmt",
        "ImGui",
		"ImGuizmo",
		"nativefiledialog",
		"opengl32.lib"
	}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "OP4_DIST"
        optimize "On"

project "opfor-editor"
    location "opfor-editor"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.fmt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stduuid}",
        "%{IncludeDir.tinygltf}",
        "opfor/src",
        "opfor/src/opfor",
        "opfor/thirdparty",
        "opfor/thirdparty/imgui",
        "opfor/thirdparty/ImGuizmo",
        "opfor/thirdparty/tinygltf",
        "opfor/thirdparty/nativefiledialog/src/include",
        "opfor/thirdparty/stduuid/include",
        "opfor/thirdparty/glew/include",
        "opfor/thirdparty/glm",
    }

    links
    {
        "opfor",
        "fmt",
        "ImGui"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "OP4_DIST"
        optimize "On"