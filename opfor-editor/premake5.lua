project "opfor-editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp"
    }

    includedirs
    {
        "include",
        "src",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stduuid}",
        "%{IncludeDir.tinygltf}",
    }

    links
    {
        "opfor",
    }

    filter "system:windows"
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

        links
        {
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
