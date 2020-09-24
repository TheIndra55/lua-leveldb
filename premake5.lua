workspace "lua-leveldb"
    architecture "x64"
    configurations {"Debug", "Release"}

project "lua-leveldb"
    kind "SharedLib"
    files {"src/**.hpp", "src/**.cpp"}
    language "C"

	targetname "leveldb"

	includedirs {"D:\\leveldb\\include", "include/"}
	libdirs {"D:\\leveldb\\build\\Debug"}
	links {"leveldb", "lua54"}

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "On"
