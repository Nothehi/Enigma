workspace "Enigma"
   architecture "x64"
   configurations { "Debug", "Release" }

project "Enigma"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   
   targetdir "build/bin/%{cfg.buildcfg}"
   objdir "build/obj/%{cfg.buildcfg}"

   files { "**.hpp", "**.cpp" }

   includedirs {
        "src"
    }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"