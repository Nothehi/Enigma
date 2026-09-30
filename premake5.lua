workspace "Enigma"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "Enigma"

   -- Shared by every project below.
   language "C++"
   cppdialect "C++20"

   targetdir "build/bin/%{cfg.buildcfg}"
   objdir "build/obj/%{cfg.buildcfg}/%{prj.name}"

   includedirs { "src" }

   warnings "Extra"
   buildoptions { "-Wpedantic", "-Wshadow", "-Wconversion", "-Wsign-conversion" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      buildoptions { "-fsanitize=address,undefined" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter {}

-- The cipher machine: pure logic, no I/O. Linked by the app and, later, by the tests.
project "EnigmaCore"
   kind "StaticLib"
   files { "src/core/**.hpp", "src/core/**.cpp" }

-- The terminal front end.
project "Enigma"
   kind "ConsoleApp"
   files { "src/app/**.hpp", "src/app/**.cpp" }
   links { "EnigmaCore" }

   filter "configurations:Debug"
      linkoptions { "-fsanitize=address,undefined" }
