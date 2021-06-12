﻿workspace "SDEngine"
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
		"libfbxsdk.lib",
		"Dinput8.lib",
		"d3d11.lib",
		"d3dcompiler.lib",
		"dxguid.lib",
		"winmm.lib",
		"comctl32.lib",
		"dxgi.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines
		{
			"_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING",
			"FBXSDK_SHARED"
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
		"SDEngine/ThirdParty/spdlog/include"
	}

	links
	{
		"SDEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines
		{
			"_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING",
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"