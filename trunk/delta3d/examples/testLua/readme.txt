To compile the Lua test, first download and compile Lua 5.0:

   http://www.lua.org/mirrors.html

Then download and compile Luabind:

   https://sourceforge.net/project/showfiles.php?group_id=77060

   (in both cases, you'll have to create your own project files that include all
    source files; be sure to use enable RTTI and C++ exceptions and use the
    multi-threaded DLL runtime)
    
Now, go to the "Directories" section of the Visual C++ "Options" menu.  Add the
Lua and Luabind include directories (e.g., "c:\lua-5.0\include" and "c:\luabind")
to the "Include" section, and their lib directories (e.g., "c:\lua-5.0\lib" and
"C:\luabind\lib") to the "Library files" section.

When running testLua, try typing the following command in the console:

   dofile('flyplane.lua')

You should see the Cessna fly around in a circle.

     --Andrzej
