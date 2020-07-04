workspace "jarp"
    architecture "x86_64"
    configurations {
        "Debug",
        "Release"
    }
    startproject "jarp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
sdllib = "%{wks.location}/jarp/ThirdParty/SDL2/lib/x64"

targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

IncludeDir = {}
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

        "%{prj.name}/Shader/**.glsl",

        "%{IncludeDir.stb}/stb_image_impl.c"
    }

    includedirs {
        "%{prj.name}/Source",
        "%{wks.location}/Sandbox/Source",

        "%{IncludeDir.stb}"
    }

    defines {
        "LEFT_HAND_COORDINATES",
        "ZERO_TO_ONE_COORDINATES"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WIN32",
            "VK_USE_PLATFORM_WIN32_KHR",
            "_CRT_SECURE_NO_WARNINGS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        files {
            "%{IncludeDir.volk}/volk.c"
        }
        includedirs {
            "%{prj.name}/ThirdParty/SDL2/include",
            "%{IncludeDir.volk}",
            "C:\\VulkanSDK\\1.2.131\\Include"
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

    filter 'files:**/Shader/**.glsl'
        buildmessage 'Compiling shaders'
        buildcommands {
            'glslangValidator "%{prj.location}/Shader/%{file.name}" -V -o "%{prj.location}/Shader/%{file.basename}.spv"'
        }
        buildinputs {
            "%{prj.name}/Shader",
        }
        buildoutputs {
            "%{prj.location}/Shader/%{file.basename}.spv"
        }

    filter "configurations:Debug"
        defines { "_DEBUG", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_LIB" }
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "SharedLib"
    language "C"
    cdialect "C99"
    staticruntime "on"

    files {
        "%{prj.name}/Source/**.c",
        "%{prj.name}/Source/**.h"
    }

    includedirs {
        "%{prj.name}/Source",
        "jarp/Source"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }

    filter 'files:**/Shader/**.glsl'
        buildmessage 'Compiling shaders'
        buildcommands {
            'glslangValidator "%{prj.location}/Shader/%{file.name}" -V -o "%{prj.location}/Shader/%{file.basename}.spv"'
        }
        buildinputs {
            "%{prj.name}/Shader",
        }
        buildoutputs {
            "%{prj.location}/Shader/%{file.basename}.spv"
        }

    filter "configurations:Debug"
        defines { "_DEBUG", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_LIB" }
        runtime "Release"
        optimize "on"

project "Game"
    location "Game"
    kind "SharedLib"
    language "C"
    cdialect "C99"
    staticruntime "on"

    files {
        "%{prj.name}/Source/**.c",
        "%{prj.name}/Source/**.h",

        "%{prj.name}/Shader/**.glsl",
    }

    includedirs {
        "%{prj.name}/Source",
        "jarp/Source"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString("HHmmssfff")).pdb'
        prebuildcommands {
            "{TOUCH} $(TargetDir)/BuildLock.tmp"
        }
        postbuildcommands {
            "{DELETE} $(TargetDir)/BuildLock.tmp"
        }

    filter 'files:**/Shader/**.glsl'
        buildmessage 'Compiling shaders'
        buildcommands {
            'glslangValidator "%{prj.location}/Shader/%{file.name}" -V -o "%{prj.location}/Shader/%{file.basename}.spv"'
        }
        buildinputs {
            "%{prj.name}/Shader",
        }
        buildoutputs {
            "%{prj.location}/Shader/%{file.basename}.spv"
        }

    filter "configurations:Debug"
        defines { "_DEBUG", "_LIB" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_LIB" }
        runtime "Release"
        optimize "on"
