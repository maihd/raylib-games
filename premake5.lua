local ROOT_DIR = path.getabsolute(".")
local BUILD_DIR = path.join(ROOT_DIR, "Build")

workspace "RaylibGames"
do
    language "C"
    location (BUILD_DIR)

    platforms { "x32", "x64", "Native" }
    configurations { "Debug", "Release" }

    filter {} 
end

project "Framework"
do
    kind "StaticLib"

    includedirs { 
        path.join(ROOT_DIR, "Framework/Include"),
    }

    files {
        path.join(ROOT_DIR, "Framework/Include/*.h"),
        path.join(ROOT_DIR, "Framework/Include/**/*.h"),
        path.join(ROOT_DIR, "Framework/Sources/*.c"),
        path.join(ROOT_DIR, "Framework/Sources/**/*.c"),
    }

    filter {}
end

function template(name, projectPath)
    project (name)
    do
        kind "ConsoleApp"

        links {
            "raylib_static",
            "winmm",
            "Framework"
        }

        includedirs {
            path.join(ROOT_DIR, "ThirdParty/Include"),
            path.join(ROOT_DIR, "Framework/Include"),
        }

        files {
            path.join(ROOT_DIR, projectPath, "*.h"),
            path.join(ROOT_DIR, projectPath, "**/*.h"),
            path.join(ROOT_DIR, projectPath, "*.c"),
            path.join(ROOT_DIR, projectPath, "**/*.c"),
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

function example(name)
    template(name, path.join("Examples", name))
end

function game(name)
    template(name, path.join("Games", name))
end

example "BasicWindow"
example "Gestures"
example "AppState"
example "Shapes"
example "Easing"
example "Textures"
example "DrawRing"

game "NeonShooter"