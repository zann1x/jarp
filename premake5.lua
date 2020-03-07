workspace "jarp"
    architecture "x86_64"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
sdllib = "%{wks.location}/jarp/ThirdParty/SDL2/lib/x64"

targetdir ("%{prj.name}/Binaries/" .. outputdir .. "/%{prj.name}")
objdir ("%{prj.name}/Intermediate/" .. outputdir .. "/%{prj.name}")

IncludeDir = {}
IncludeDir["glm"] = "jarp/ThirdParty/glm"
IncludeDir["SDL"] = "jarp/ThirdParty/SDL2/include"
IncludeDir["stb"] = "jarp/ThirdParty/stb"
IncludeDir["volk"] = "jarp/ThirdParty/volk"

project "jarp"
    location "jarp"
    kind "ConsoleApp"
    language "C"
    cdialect "C99"
    staticruntime "on"

    files {
        "%{prj.name}/Source/**.c",
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Shaders/**.glsl",

        "%{IncludeDir.stb}/stb_image_impl.c",
        "%{IncludeDir.volk}/volk.c"
    }

    includedirs {
        "%{prj.name}/Source",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tinyobjloader}",
        "%{IncludeDir.volk}"
    }

    defines {
        "GLM_FORCE_RADIANS",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WINDOWS",
            "VK_USE_PLATFORM_WIN32_KHR",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        includedirs {
            "%{prj.name}/ThirdParty/SDL2/include",
            "C:\\VulkanSDK\\1.2.131.1\\Include"
        }
        libdirs {
            sdllib
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
        buildmessage 'Compiling shaders'
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
        defines { "_DEBUG", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_LIB" }
        runtime "Release"
        optimize "on"
