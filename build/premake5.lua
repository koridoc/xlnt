solution "xlnt"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    location ("./" .. _ACTION)
    configuration "Debug"
        flags { "Symbols" }
	optimize "Off"
    configuration "Release"
        optimize "Full"

project "xlnt.test"
    kind "ConsoleApp"
    language "C++"
    targetname "xlnt.test"
    includedirs { 
       "../include",
       "../third-party/pugixml/src",
       "../third-party/cxxtest"
    }
    files { 
       "../tests/*.hpp",
       "../tests/runner-autogen.cpp"
    }
    links { 
	"pugixml",
        "xlnt"
    }
    prebuildcommands { "../../third-party/cxxtest/bin/cxxtestgen --runner=ErrorPrinter -o ../../tests/runner-autogen.cpp ../../tests/*.hpp" }
    flags { 
       "Unicode",
       "NoEditAndContinue",
       "NoManifest",
       "NoPCH"
    }
    configuration "Debug"
	targetdir "../bin"
    configuration "Release"
        flags { "LinkTimeOptimization" }
	targetdir "../bin"
    configuration "windows"
        defines { "WIN32" }
	links { "Shlwapi" }
	postbuildcommands { "..\\..\\bin\\xlnt.test" }
    configuration "not windows"
        postbuildcommands { "../../bin/xlnt.test" }
        buildoptions { 
            "-std=c++11",
            "-Wno-unknown-pragmas"
        }
    configuration { "not windows", "Debug" }
        buildoptions { "-ggdb" }

project "xlnt"
    kind "StaticLib"
    language "C++"
    warnings "Extra"
    targetdir "../lib/"
    links { 
	"pugixml"
    }
    includedirs { 
       "../include/xlnt",
       "../third-party/pugixml/src"
    }
    files {
       "../source/**.cpp",
       "../source/**.hpp",
       "../include/xlnt/**.hpp"
    }
    flags { 
       "Unicode",
       "NoEditAndContinue",
       "NoManifest",
       "NoPCH"
    }
    configuration "Debug"
        flags { "FatalWarnings" }
    configuration "windows"
        defines { 
	   "WIN32",
	   "_CRT_SECURE_NO_WARNINGS"
	}
    configuration "not windows"
        buildoptions { 
            "-std=c++11",
            "-Wno-unknown-pragmas"
        }
    configuration { "not windows", "Debug" }
        buildoptions { "-ggdb" }

project "pugixml"
    kind "StaticLib"
    language "C++"
    warnings "Off"
    targetdir "../lib/"
    includedirs { 
       "../third-party/pugixml/src"
    }
    files {
       "../third-party/pugixml/src/pugixml.cpp"
    }
    flags { 
       "Unicode",
       "NoEditAndContinue",
       "NoManifest",
       "NoPCH"
    }
    configuration "windows"
        defines { "WIN32" }
