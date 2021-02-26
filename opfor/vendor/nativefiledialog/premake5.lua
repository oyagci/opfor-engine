project "nativefiledialog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/nfd_common.c",
		"src/nfd_common.h",
	}


	includedirs {
		"src/include",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
			"src/nfd_win.cpp",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"

		buildoptions {"`pkg-config --cflags gtk+-3.0`"}
		linkoptions {"`pkg-config --libs gtk+-3.0`"}

		files {
			"src/nfd_gtk.c",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
