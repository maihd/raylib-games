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

project "BasicWindow"
do
    kind "ConsoleApp"

    links {
        "raylib_static",
        "winmm"
    }

    includedirs {
        path.join(ROOT_DIR, "ThirdParty/Include")
    }

    files {
        path.join(ROOT_DIR, "Examples/BasicWindow/Main.c")
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