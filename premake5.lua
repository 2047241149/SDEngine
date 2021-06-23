workspace "SDEngine"
	architecture "x64"
	startproject "TestGame"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

include "SDEngine/ThirdParty/imgui"

project "SDEngine"
	location "SDEngine"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/Source/SDEngine",
		"%{prj.name}/ThirdParty/spdlog/include",
		"%{prj.name}/ThirdParty/fbxsdk/2017.1/include",
		"%{prj.name}/ThirdParty/imgui",
	}

	links
	{
		"imgui",
		"libfbxsdk-mt.lib",
		"dxguid.lib",
		"dxgi.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
		}

		postbuildcommands
		{
			("{COPY} ThirdParty/fbxsdk/2017.1/lib/x64/debug/libfbxsdk.dll ../bin/" .. outputdir .. "/TestGame"),
		}

	filter "configurations:Debug"
		symbols "On"
		--buildoptions "/MDd"

	filter "configurations:Release"
		optimize "On"


project "TestGame"
	location "TestGame"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"SDEngine/Source",
		"SDEngine/SDEngine",
		"SDEngine/ThirdParty/spdlog/include",
		"SDEngine/ThirdParty/fbxsdk/2017.1/include",
		"SDEngine/ThirdParty",
	}

	links
	{
		"SDEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
		}

	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		optimize "On"
		runtime "Debug"