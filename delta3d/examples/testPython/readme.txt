To compile the Python test, first install Python 2.3:

   http://www.python.org/2.3.4/

Then obtain the Boost library from SourceForge:

   http://sourceforge.net/project/showfiles.php?group_id=7586

Unzip Boost and compile Boost.Python using:

   C:\boost_1_31_0\libs\python\build\VisualStudio\boost_python.dsw

Put the resulting boost_python.dll and boost_python_debug.dll in your path.

Now, go to the "Projects/VC++ Directories" section of the Visual C++ "Options" menu.  Add the
Python and Boost include directories (e.g., "c:\Python23\include" and "c:\boost_1_31_0")
to the "Include" section, and their lib directories (e.g., "c:\Python23\libs" and
"C:\boost_1_31_0\libs\python\build\bin-stage") to the "Library files" section.

When running testPython, try typing the following command in the console:

   execfile('flyplane.py')

You should see the Cessna fly around in a circle.  You can press ^C to return to the
Python interpreter.  To exit the application, type ^Z and Enter.

     --Andrzej
