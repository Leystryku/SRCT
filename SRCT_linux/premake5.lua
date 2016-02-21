solution "MySolution"
   language "C++"
   configurations { "Debug", "Release" }

project "SRCT_linux"
   kind "ConsoleApp"
   language "C++"
   includedirs { "include" }
   files { "src/**.h", "src/**.cpp" }

   configuration "Debug"
      flags { "Symbols" }
      defines { "_DEBUG" }

   configuration "Release"
      flags { "Optimize" }
      defines { "NDEBUG" }