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

testpathedcamera.py: flies the camera along a series of waypoints.

testPythongui.py: opens a Tk GUI and displays the helo.
                  Bug: user may need to click on the menu before helo appears.

testpython.py: also flies a helo in a circle, but should be run straight from
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

Check if the following environement variables are set:
  PYTHON_ROOT: directory of Python installation
  PYTHON_LIB_PATH: %PYTHON_ROOT%/libs
  PYTHON_VERSION: version of your Python installation
  PYTHONPATH: environement variable contains the directory with the resulting
              .pyd libraries (most likely delta3d/bin).

Build the Delta3D Python bindings:
- Open delta3d/src/python/dtPython.sln
- Build.

To use pre-built testPython: 
- Go to examples/testPython/Release.
- Double-click testPython.exe.
- At the Python prompt inside the console/terminal type:
   execfile('../flyhelo.py')

To build from source:
- Open examples/testPython/testPython.sln.
- Go to the Tools->Options->Projects->VC++ Directories and select 
  "Show directories for: Include files".
- Add the Boost include directorie (e.g., "c:\boost_1_31_0") to the "Include" 
  section.
- Boost lib directories (e.g., "C:\boost_1_31_0\libs\python\build\bin-stage") 
  to the "Library files" section.
- Build testPython.
- Start testPython.
- At the Python prompt inside the console/terminal type:
   execfile('flyhelo.py')


This should make the helo fly in a circle.

Hit escape to exit.

testpython.py: 
- This file is a seperate Python script that uses Delta3D. 
- To use it, double-click on testpython.py.
- The resulting window should also show a helo flying in a circle.

testpathedcamera.py:
- This script shows how to move a camera along a series of waypoints.
- To use it, double-click on testpathedcamera.py.

testpythongui.py:
- This script uses the Tk toolkit (integrated with Python) to make a Delta3D Widget and
  display the standard helo.
- To use it, double-click on testpythongui.py.

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
- Set env. var PYTHON_ROOT to your your python installation (usually /usr or /usr/local)
- Set env. var PYTHON_VERSION to the 2-part major python version (e.g. 2.3 or 2.4)
- Go to boost_1_31_0/libs/python/build.
- Run 'bjam -sTOOLS=gcc'.
- Copy resulting shared objects to delta3d/ext/lib (non-debug version is
  already supplied, but hey, you want your own, right?):
     'cp -d libboost_python.so* $DELTA_ROOT/ext/lib'
     'cp -d libboost_python_debug.so* $DELTA_ROOT/ext/lib'

Note: If you wish to place the boost_python libraries in a different location,
      make sure to edit delta3d/src/python/CMakeLists.txt and add that location
      to the LINK_DIRECTORTIES list.

Build the Python bindings for Delta3D engine:
- Go to the delta3d root directory.
- Run 'scons python'
- Run 'scons install'

Set the PYTHONPATH environement variable to contain $DELTA_LIB.

The testPython example should be built automictally by the above command.

Run 'testPython'.

At the Python prompt inside the console/terminal type:
   execfile('flyhelo.py')

This should make the helo fly in a circle. 

Hit escape to exit.

testpython.py: 
- This file is a seperate Python script that uses Delta3D. 
- To use it, from examples/testPython type:
     python testpython.py.
- The resulting window should also show a helo flying in a circle.

testpathedcamera.py:
- This script shows how to move a camera along a series of waypoints.
- To use it, from examples/testPython type:
     python testpathedcamera.py.

testpythongui.py:
- This script uses the Tk toolkit to make a quick GUI.
- It is unsupported on Linux for now, due to a X11 crash.

