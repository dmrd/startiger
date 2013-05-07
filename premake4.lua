-- Project ----------------------------------------------------------------------------------

solution "startiger"
configurations { "Debug", "Release" }

project "startiger"
targetname "startiger"
kind "ConsoleApp"
language "C++"


-- Files ------------------------------------------------------------------------------------

files { "./src/**.h", "./src/**.cpp" }


-- Include/library directories and libraries ------------------------------------------------

includedirs { 
    -- project
    "./src",

    -- libs
    "./lib"
}

libdirs  {
    -- libs
    "./lib/jpeg",
    "./lib/glut"
}


-- Links ------------------------------------------------------------------------------------

linkoptions "-ljpeg"

if os.is("linux") then
    links { "glut", "GLU", "GL" }
elseif os.is("windows") then
    links { "glut32", "glu32", "opengl32" }
else
    linkoptions "-framework GLUT -framework OpenGL"
end


-- Configurations ---------------------------------------------------------------------------

configuration "Debug"
    flags { "Symbols" }
    defines { "DEBUG", "_DEBUG" }

    objdir "obj/debug"
    targetdir "bin"
    targetsuffix "_d"

configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }

    objdir "obj/release"
    targetdir "bin"


