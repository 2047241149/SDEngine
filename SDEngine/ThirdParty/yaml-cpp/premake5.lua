project "yaml-cpp"
    kind "StaticLib"
	language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"src/**.h",
		"src/**.cpp",
		"include/**.h",
    }

	includedirs
	{
		"include",
	}

    filter "system:windows"
        systemversion "latest"
		cppdialect "C++17"
		staticruntime "on"

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"
