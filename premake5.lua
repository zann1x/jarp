workspace "jarp"
    architecture "x64"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
vulkansdk = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["glm"] = "jarp/Vendor/glm"
IncludeDir["tinyobjloader"] = "jarp/Vendor/tinyobjloader"
IncludeDir["SDL"] = "jarp/Vendor/SDL2-2.0.9/include"
IncludeDir["stb"] = "jarp/Vendor/stb"
IncludeDir["Vulkan"] = vulkansdk .. "/Include"

-- group "Dependencies"
--     include "jarp/Vendor/SDL2-2.0.9"
-- group ""

project "jarp"
    location "jarp"
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

        "%{prj.name}/Shaders/**.glsl"
    }

    includedirs {
        "%{prj.name}/Source",

        "%{IncludeDir.glm}",
        "%{IncludeDir.SDL}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tinyobjloader}",
        "%{IncludeDir.Vulkan}"
    }

    libdirs {
        "%{prj.location}/Vendor/SDL2-2.0.9/lib/x64",
        vulkansdk .. "/Lib"
    }

    links {
        "SDL2",
        "SDL2main",
        "vulkan-1.lib"
    }

    defines {
        "GLM_FORCE_RADIANS",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "SDL_MAIN_HANDLED"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "VK_USE_PLATFORM_WIN32_KHR",
            "GLFW_INCLUDE_NONE",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        postbuildcommands {
			-- Copy the SDL2 dll to the bin folder
			'{COPY} "%{prj.location}/Vendor/SDL2-2.0.9/lib/x64/SDL2.dll" "%{cfg.targetdir}"'
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
