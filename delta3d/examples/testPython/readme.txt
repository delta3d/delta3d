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
To use pre-built testPython: 
- Go to examples/testPython/Release.
- Double-click testPython.exe.
- At the Python prompt inside the console/terminal type:
   execfile('../flyhelo.py')

To build from source:
- Open examples/testPython/testPython.sln.
- Go to the Tools->Options->Projects->VC++ Directories and select 
  "Show directories for: Include files".
- Add the Boost include directorie (e.g., "c:\boost_1_32_0") to the "Include" 
  section.
- Boost lib directories (e.g., "C:\boost_1_32_0\libs\python\build\bin-stage") 
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

Run:

scons boost=/path/to/boost examples/testPython
 or
scons boost=/path/to/boost python

The testPython example should be built automictally by either of 
the above commands.

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
