########################################
#                                      #
# Delta3D Examples: testPython         #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtPython

Purpose
-------
To demonstrate the Python interface to Delta3D. There are several scripts:

flyhelo.py: should be run from the prompt within testPython to fly the 
            helicopter in a circle.

testPathedCamera.py: flies the camera along a series of waypoints.

testPythonGUI.py:

testPython.py: also flies a helo in a circle, but should be run straight from
               Python instead of within the testPython C++ application.

See instructions for each platform for details.

Instructions
------------

Win32
=====
Install Python 2.3 from:
   http://www.python.org/2.3.4/

Obtain the Boost library from SourceForge (package 'boost'):
   http://sourceforge.net/projects/boost/

Decompress Boost.

Open boost_1_31_0/libs/python/build/VisualStudio/boost_python.dsw.

Build boost_python.

Make sure boost_python.dll and boost_python_debug.dll are within a directory in your PATH
   evironement variable.

Build the Delta3D Python bindings:
- Open delta3d/src/python/dtPython.sln
- Build.
- Make sure the PYTHONPATH environement variable contains the directory with the resulting
  .pyd libraries (most likely delta3d/bin).

To use pre-built testPython: 
- Go to examples/testPython/Release.
- Double-click testPython.exe.
- At the Python prompt inside the console/terminal type:
   execfile('../flyhelo.py')


To build from source:
- Open examples/testPython/testPython.sln.
- Go to the "Projects/VC++ Directories" section of the Visual C++ "Options" menu.  
- Add the Python and Boost include directories (e.g., "c:\Python23\include" and "c:\boost_1_31_0")
  to the "Include" section.
- Add the Python and Boost lib directories (e.g., "c:\Python23\libs" and 
  "C:\boost_1_31_0\libs\python\build\bin-stage") to the "Library files" section.
- Build testPython.
- Start testPython.
- At the Python prompt inside the console/terminal type:
   execfile('flyhelo.py')


This should make the helo fly in a circle.

Hit escape to exit.

testPython.py: 
- This file is a seperate Python script that uses Delta3D. 
- To use it, double-click on testPython.py.
- The resulting window should also show a helo flying in a circle.

testPathedCamera.py:
- This script shows how to move a camera along a series of waypoints.
- To use it, double-click on testPathCamrea.py.

testPythonGUI.py:
- This script uses the Tk toolkit (integrated with Python) to make a Delta3D Widget and
  display the standard helo.
- To use it, double-click on testPythonGUI.py.

Linux
=====

Install Python:
- Check if you have Python installed, type:
     python -V
- If it prints out the version number, you are set. 
- Else install Python 2.3:
     http://www.python.org/2.3.4/

Install Boost Python:
- Obtain the Boost library from SourceForge (package 'boost'):
     http://sourceforge.net/projects/boost/
- Obtain Boost Jam (package 'boost-jam').
- Decompress the BoostJam archive and place the file 'bjam' in your PATH.
- Go to boost_1_31_0/libs/python/build.
- Run 'bjam -sTOOLS=gcc'.
- Copy resulting shared objects to /usr/local/lib:
     'cp -d libboost_python.so* /usr/local/lib'
     'cp -d libboost_python_debug.so* /usr/local/lib'

Note: If you wish to place the boost_python libraries in a different location,
      make sure to edit delta3d/src/python/CMakeLists.txt and add that location
      to the LINK_DIRECTORTIES list.

Build the Python bindings for Delta3D engine:
- Go to delta3d/src/python.
- Run 'cmake .'
- Run 'make; make install'
- Run 'install_linux_python'

Build testPython example:
- Go to examples/testPython.
- Run 'cmake .'.
- Run 'make'.

Run 'testPython'.

At the Python prompt inside the console/terminal type:
   execfile('flyhelo.py')

This should make the helo fly in a circle. 

Hit escape to exit.

testPython.py: 
- This file is a seperate Python script that uses Delta3D. 
- To use it, from examples/testPython type:
     python testPython.py.
- The resulting window should also show a helo flying in a circle.

testPathedCamera.py:
- This script shows how to move a camera along a series of waypoints.
- To use it, from examples/testPython type:
     python testPathedCamera.py.

testPythonGUI.py:
- This script uses the Tk toolkit to make a quick GUI.
- It is unsupported on Linux for now, due to a X11 crash.

