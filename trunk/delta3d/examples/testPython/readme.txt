To compile the Python test, first install Python 2.2:

   http://www.python.org/ftp/python/2.2.3/Python-2.2.3.exe

Then obtain the Boost library from SourceForge:

   http://sourceforge.net/project/showfiles.php?group_id=7586

Unzip Boost and compile Boost.Python using:

   C:\boost-1.30.2\libs\python\build\VisualStudio\boost_python.dsw

   ...but before doing so, add BOOST_MSVC_STD_ITERATOR to the list of
   preprocessor definitions under Settings / C/C++ / Preprocessor /
   Preprocessor definitions for both the Debug and Release configurations.

Put the resulting boost_python.dll and boost_python_debug.dll in your path.

Now, go to the "Directories" section of the Visual C++ "Options" menu.  Add the
Python and Boost include directories (e.g., "c:\Python22\include" and "c:\boost-1.30.2")
to the "Include" section, and their lib directories (e.g., "c:\Python22\libs" and
"C:\boost-1.30.2\libs\python\build\bin-stage") to the "Library files" section.

When running testPython, try typing the following command in the console:

   execfile('flyplane.py')

You should see the Cessna fly around in a circle.  You can press ^C to return to the
Python interpreter.

     --Andrzej
