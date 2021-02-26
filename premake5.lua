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
	cppdialect "C++17"

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
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

	filter "system:linux"

        defines
        {
            "OP4_PLATFORM_LINUX",
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
	cppdialect "C++17"

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
    }

    links
    {
        "opfor",
        "fmt",
        "ImGui",
		"uuid",
		"GLFW",
		"GLAD",
        "ImGui",
		"ImGuizmo",
		"nativefiledialog",
    }

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_PLATFORM_OPENGL"
        }

	filter "system:linux"

		-- linkoptions { "`pkg-config --libs gtk+-3.0`" }
		toolset "clang"

        defines
        {
            "OP4_PLATFORM_LINUX",
            "OP4_PLATFORM_OPENGL"
        }

		links {
			"dl",
			"m",
			"X11",
			"pthread",
			"GL"
		}
		linkoptions { "-lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0" }

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "OP4_DIST"
        optimize "On"
