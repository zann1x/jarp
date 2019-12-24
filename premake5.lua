workspace "jarp"
    architecture "x86_64"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
sdllib = "%{wks.location}/jarp/ThirdParty/SDL2/lib/x64"

IncludeDir = {}
IncludeDir["glm"] = "jarp/ThirdParty/glm"
IncludeDir["SDL"] = "jarp/ThirdParty/SDL2/include"
IncludeDir["spdlog"] = "jarp/ThirdParty/spdlog/include"
IncludeDir["stb"] = "jarp/ThirdParty/stb"
IncludeDir["tinyobjloader"] = "jarp/ThirdParty/tinyobjloader"
IncludeDir["volk"] = "jarp/ThirdParty/volk"

project "jarp"
    location "jarp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("jarp/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("jarp/Intermediate/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/Source/**.c",
        "%{prj.name}/Source/**.cpp",
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.hpp",
        "%{prj.name}/Shaders/**.glsl",

        "%{IncludeDir.stb}/stb_image.cpp",
        "%{IncludeDir.tinyobjloader}/tiny_obj_loader.cpp",
        "%{IncludeDir.volk}/volk.cpp"
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

    filter "system:linux or configurations:gmake2"
        defines {
            "JARP_PLATFORM_LINUX"
        }
        removefiles {
            "%{prj.name}/Source/Platform/Windows/**"
        }
        includedirs {
            "`sdl2-config --cflags`",
            "/usr/include/vulkan"
        }
        buildoptions {
            "`sdl2-config --cflags`"
        }
        linkoptions { 
            "`sdl2-config --libs`",
        }

    filter "system:windows"
        systemversion "latest"

        defines {
            "JARP_PLATFORM_WINDOWS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        removefiles {
            "%{prj.name}/Source/Platform/Linux/**"
        }
        includedirs {
            "%{prj.name}/ThirdParty/SDL2/include",
            "C:\\VulkanSDK\\1.1.126\\Include"
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
        defines { "_DEBUG", "_CONSOLE" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE" }
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("Sandbox/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("Sandbox/Intermediate/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/Source/**.c",
        "%{prj.name}/Source/**.cpp",
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.hpp",

        "%{prj.name}/Shaders/**.glsl",
    }

    includedirs {
        "jarp/Source",
        "%{prj.name}/Source",

        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}"
    }

    links {
        "jarp"
    }

    filter "system:linux or configurations:gmake2"
        defines {
            "JARP_PLATFORM_LINUX"
        }
        linkoptions { 
            "`sdl2-config --libs`"
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
        defines { "_DEBUG", "_CONSOLE" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_CONSOLE" }
        runtime "Release"
        optimize "on"
