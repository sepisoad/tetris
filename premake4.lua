solution "test"
  configurations { "Debug", "Release" }

  project "game"
    language "C"
    -- kind "ConsoleApp"
    kind "WindowedApp"
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
    if os.get() == "windows" then
      postbuildcommands {}
    else
      postbuildcommands {}
    end

  configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize", "ExtraWarnings" }