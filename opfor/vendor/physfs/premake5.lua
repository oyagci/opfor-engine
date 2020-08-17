project "PhysFS"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/physfs.c",
		"src/physfs_byteorder.c",
		"src/physfs_unicode.c",
		"src/physfs_platform_posix.c",
		"src/physfs_platform_unix.c",
		"src/physfs_platform_windows.c",
		"src/physfs_platform_os2.c",
		"src/physfs_platform_qnx.c",
		"src/physfs_archiver_dir.c",
		"src/physfs_archiver_unpacked.c",
		"src/physfs_archiver_grp.c",
		"src/physfs_archiver_hog.c",
		"src/physfs_archiver_7z.c",
		"src/physfs_archiver_mvl.c",
		"src/physfs_archiver_qpak.c",
		"src/physfs_archiver_wad.c",
		"src/physfs_archiver_zip.c",
		"src/physfs_archiver_slb.c",
		"src/physfs_archiver_iso9660.c",
		"src/physfs_archiver_vdf.c",
		"src/physfs_platform_apple.m",
		"src/physfs_platform_winrt.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"