solution "test"
  configurations { "Debug", "Release" }

  project "game"
    language "C"
    kind "ConsoleApp"
    files { "src/**.c" }
    -- buildoptions { "-Werror" }
    if os.get() == "windows" then
      links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "SDL2_ttf", "SDL2_mixer" }
    else
      links { "SDL2main", "SDL2", "SDL2_image", "SDL2_ttf", "SDL2_mixer" }
    end

  configuration "Debug"
    defines { "DEBUG", "MEM_DEBUG" }
    flags { "Symbols", "ExtraWarnings" }

  configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize", "ExtraWarnings" }