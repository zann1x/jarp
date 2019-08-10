workspace "jarp"
    architecture "x64"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
sdllib = "%{wks.location}/jarp/ThirdParty/SDL2/lib/x64"
vulkansdk = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["glm"] = "jarp/ThirdParty/glm"
IncludeDir["imgui"] = "jarp/ThirdParty/imgui"
IncludeDir["tinyobjloader"] = "jarp/ThirdParty/tinyobjloader"
IncludeDir["SDL"] = "jarp/ThirdParty/SDL2/include"
IncludeDir["spdlog"] = "jarp/ThirdParty/spdlog/include"
IncludeDir["stb"] = "jarp/ThirdParty/stb"
IncludeDir["Vulkan"] = vulkansdk .. "/Include"

project "jarp"
    location "jarp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "jarppch.h"
    pchsource "jarp/Source/jarppch.cpp"

    files {
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.hpp",
        "%{prj.name}/Source/**.cpp",

        "%{prj.name}/Shaders/**.glsl"
    }

    includedirs {
        "%{prj.name}/Source",

        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tinyobjloader}",
        "%{IncludeDir.Vulkan}"
    }

    libdirs {
        vulkansdk .. "/Lib"
    }

    links {
        "vulkan-1"
    }

    defines {
        "GLM_FORCE_RADIANS",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
    }

    filter "system:windows"
        systemversion "latest"

        includedirs {
            "%{IncludeDir.SDL}",
        }
        libdirs {
            sdllib
        }
        defines {
            "JARP_PLATFORM_WINDOWS",
            "VK_USE_PLATFORM_WIN32_KHR",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        links {
            "SDL2",
            "SDL2main"
        }
        postbuildcommands {
			-- Copy the SDL2 dll to the bin folder
			'{COPY} "' .. sdllib .. '/SDL2.dll" "%{cfg.targetdir}"'
		}

    filter 'files:**/Shaders/**.glsl'
        buildcommands {
            'glslangValidator "%{prj.location}/Shaders/%{file.name}" -V -o "%{prj.location}/Shaders/%{file.basename}.spv"'
        }
        buildinputs {
            "%{prj.name}/Shaders",
        }
        buildoutputs {
            "%{prj.location}/Shaders/%{file.basename}.spv"
        }

    filter "configurations:Debug"
        defines { "_DEBUG", "_CONSOLE", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE", "_LIB" }
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.hpp",
        "%{prj.name}/Source/**.cpp",
    }

    includedirs {
        "jarp/Source",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}"
    }

    links {
        "jarp"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WINDOWS"
        }
        postbuildcommands {
			-- Copy the SDL2 dll to the bin folder
			'{COPY} "' .. sdllib .. '/SDL2.dll" "%{cfg.targetdir}"'
        }

    filter "configurations:Debug"
        defines { "_DEBUG", "_CONSOLE", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE", "_LIB" }
        runtime "Release"
        optimize "on"
    