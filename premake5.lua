local ROOT_DIR = path.getabsolute(".")
local BUILD_DIR = path.join(ROOT_DIR, "Build")
local THIRDPARTY_DIR = path.join(ROOT_DIR, "ThirdParty")

workspace "RaylibGames"
do
    language "C++"
    location (BUILD_DIR)

    platforms { "x32", "x64" }
    configurations { "Debug", "Release" }

    --cdialect "c99"
    --compileas "C++"
    --staticruntime "On"

    flags {
        "NoPCH"
    }

    filter { "configurations:Debug"}
    do
        defines {
            "DEBUG"
        }
    end

    filter { "configurations:Release"}
    do
        defines {
            "RELEASE"
        }
    end

    filter {} 
end

project "Framework"
do
    kind "StaticLib"

    links {
    }

    includedirs { 
        path.join(ROOT_DIR, "Framework/Include"),
        path.join(ROOT_DIR, "ThirdParty/Include"),
    }

    files {
        path.join(ROOT_DIR, "Framework/Include/*.h"),
        path.join(ROOT_DIR, "Framework/Include/**/*.h"),
        path.join(ROOT_DIR, "Framework/Sources/*.c"),
        path.join(ROOT_DIR, "Framework/Sources/**/*.c"),
        path.join(ROOT_DIR, "Framework/Sources/*.cpp"),
        path.join(ROOT_DIR, "Framework/Sources/**/*.cpp"),

        path.join(THIRDPARTY_DIR, "Include/raylib.h"),
        path.join(THIRDPARTY_DIR, "Include/raymath.h"),
    }

    filter {}
end

local function template(name, projectPath, shouldHideConsole)
    project (name)
    do
        if not shouldHideConsole then
            kind "ConsoleApp"
        else 
            filter { "configurations:Debug" }
            do
                kind "ConsoleApp"
            end

            filter { "configurations:Release" }
            do
                kind "WindowedApp"
            end
            
            filter {}
        end

        links {
            "Framework",
            "raylib_static",
            "winmm",
        }

        includedirs {
            path.join(ROOT_DIR, "Framework/Include"),
            path.join(ROOT_DIR, "ThirdParty/Include"),
        }

        files {
            path.join(ROOT_DIR, projectPath, "*.h"),
            path.join(ROOT_DIR, projectPath, "**/*.h"),
            path.join(ROOT_DIR, projectPath, "*.c"),
            path.join(ROOT_DIR, projectPath, "**/*.c"),
            path.join(ROOT_DIR, projectPath, "*.cpp"),
            path.join(ROOT_DIR, projectPath, "**/*.cpp"),
        }

        defines {
            "GRAPHICS_API_OPENGL_33",
            "PLATFORM_DESKTOP"
        }

        filter { "platforms:x32" }
        do
            libdirs { path.join(ROOT_DIR, "ThirdParty/Library/Win32") }
        end

        filter { "platforms:x64" }
        do
            libdirs { path.join(ROOT_DIR, "ThirdParty/Library/Win64") }
        end

        filter {}
    end
end

local function example(name)
    template(name, path.join("Examples", name))
end

local function game(name)
    template(name, path.join("Games", name), true)
end

example "BasicWindow"
example "Gestures"
example "AppState"
example "Shapes"
example "Easing"
example "Textures"
example "DrawRing"
example "LiquidSurface2D"
example "MeshDeformation2D"
example "Todos"
example "UI"
example "ECS"
example "FLECS"

game "NeonShooter"