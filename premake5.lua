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

group "Dependencies"
    include "opfor/vendor/glfw"
    include "opfor/vendor/glad"
    include "opfor/vendor/fmt"
    include "opfor/vendor/imgui"
    include "opfor/vendor/ImGuizmo"
    include "opfor/vendor/nativefiledialog"
group ""


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
    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Dist"
        defines "OP4_DIST"
        optimize "On"

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_BACKEND_OPENGL"
        }

 filter "system:linux"
        defines
        {
            "OP4_PLATFORM_LINUX",
            "OP4_BACKEND_OPENGL"
        }

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
        "opfor/include",
        "opfor/src",
    }

    links
    {
        "opfor",
    }

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "OP4_PLATFORM_WINDOWS",
            "OP4_BACKEND_OPENGL"
        }

    filter "system:linux"
        defines
        {
            "OP4_PLATFORM_LINUX",
            "OP4_BACKEND_OPENGL"
        }

        links {
            "dl",
            "m",
            "X11",
            "pthread",
            "GL"
        }
        linkoptions {"`pkg-config --libs gtk+-3.0`"}

    filter "configurations:Release"
        defines "OP4_RELEASE"
        optimize "On"

    filter "configurations:Debug"
        defines "OP4_DEBUG"
        symbols "On"

    filter "configurations:Dist"
        defines "OP4_DIST"
        optimize "On"
