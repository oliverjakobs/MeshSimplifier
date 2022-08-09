workspace "Model"
    architecture "x64"
    startproject "Model"

    configurations
    {
        "Debug",
        "OptimizedDebug",
        "Release"
    }

    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:OptimizedDebug"
        runtime "Debug"
        symbols "On"
        optimize "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

output_dir = "%{cfg.buildcfg}"

group "Packages"

include "packages/glfw.lua"
include "packages/Ignis.lua"
include "packages/imgui.lua"

group ""

project "Model"
    kind "ConsoleApp"
    language "C++"
    staticruntime "On"
    
    targetdir ("build/bin/" .. output_dir .. "/%{prj.name}")
    objdir ("build/bin-int/" .. output_dir .. "/%{prj.name}")

    files
    {
        --Source
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        --Resources
        "res/shaders/**.vert",
        "res/shaders/**.frag"
    }

    links
    {
        "GLFW",
        "Ignis",
        "ImGui",
        "opengl32"
    }

    includedirs
    {
        "src",
        "packages/glfw/include",
        "packages/Ignis/src",
        "packages/glm/",
        "packages/imgui",
    }

    filter "system:linux"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
