workspace "Enigma"
   architecture "x64"
   configurations { "Debug", "Release" }

project "Enigma"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   targetdir "build/bin/%{cfg.buildcfg}"
   objdir "build/obj/%{cfg.buildcfg}"

   files { "src/**.hpp", "src/**.cpp" }

   includedirs { "src" }

   warnings "Extra"
   buildoptions { "-Wpedantic", "-Wshadow", "-Wconversion", "-Wsign-conversion" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      buildoptions { "-fsanitize=address,undefined" }
      linkoptions { "-fsanitize=address,undefined" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
