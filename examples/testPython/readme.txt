########################################
#                                      #
# Delta3D Examples: testPython         #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtPython, dtUtil

Purpose
-------
To demonstrate the Python interface to Delta3D. There are several scripts:

flyhelo.py:
- Should be triggered from testPython by hitting the 'S' key to fly the  helicopter in a circle.

testpathedcamera.py:
- Flies the camera along a series of waypoints.

testpythongui.py:
- Opens a Tk GUI and displays the helo. 
- Bug: user may need to click on the menu before helo appears.
- Note: this doens't work on Linux due to X11/Tk/Producer problems.

testpython.py:
- Flies a helo in a circle.
- This should be run straight from Python instead of being triggered
  from the testPython C++ application.

There are a few other examples re-implemented in Python:
- examples/testLights/testlights.py
- examples/testMap/testmap.py
- examples/testSound/testsound.py

Instructions
------------
Note: Set the environment variable PYTHONPATH to the folder that contains
      the Delta3D .pyd files.  Otherwise the scripts will not be able to
      import the modules.

Win32 Source:
- Go to the Tools->Options->Projects->VC++ Directories and select 
  "Show directories for: Include files".
- Add the Boost and Python include directories (e.g., "c:\boost_1_32_0",
  "C:\Program Files\Python2.4\include") to the "Include" section.
- Add the Boost and Python lib directories (e.g., "C:\boost_1_32_0\libs\python\build\bin-stage", 
  "C:\Program Files\Python2.4\libs") to the "Library files" section.
- Build testPython.
- Start testPython.

Win32 Binary:
- Go to bin.
- Double-click testPython.exe.


Linux Source:
- To build only testPython, from the root delta3d folder:
  scons && scons bin/testPython

Linux Binary:
- cd bin
- ./testPython

Controls
--------
<Esc>  - Exit
's'    - Triggers flyhelo.py and makes the helo fly in a circle.
