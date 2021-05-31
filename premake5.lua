workspace "SDEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

project "SDEngine"
	location "SDEngine"
	kind "SharedLib"
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
		"%{prj.name}/ThirdParty/spdlog/include",
		"%{prj.name}/ThirdParty/fbxsdk/2017.1/include",
	}

	links
	{
		"libfbxsdk.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "10.0.17763.0"

		defines
		{
			"SD_BUILD_DLL",
			"FBXSDK_SHARED"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/TestGame"),
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
		"SDEngine/ThirdParty/spdlog/include"
	}

	links
	{
		"SDEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "10.0.17763.0"

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"