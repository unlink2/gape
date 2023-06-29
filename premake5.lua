NAME="gape"

workspace (NAME)
   configurations { "Debug", "Release" }

project ("lib"..NAME)
   kind "SharedLib"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"
   links { }
   buildoptions { "-Wall", "-pedantic" }

   files { "include/lib"..NAME.."/**.h", "src/lib"..NAME.."/**.c" }
   includedirs { "include" }
   removefiles { "**/test/**" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"


project (NAME)
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"
   links { "argtable2" }
   -- links { "lib"..NAME }
   buildoptions { "-Wall", "-pedantic" }

   files { "include/**.h", "src/**.c" }
   includedirs { "include" }
   removefiles { "**/test/**" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"


project ("test"..NAME)
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"
   links { "cmocka" }
   buildoptions { "-Wall", "-pedantic" }

   files { "include/lib"..NAME.."/**.h", "src/lib"..NAME.."/**.c" }
   includedirs { "include" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

