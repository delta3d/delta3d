########################################
#                                      #
# Delta3D Examples: testWeather        #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the weather features of the dtABC module. Fog can be increased
or decreased by setting the visibility. The amount of clouds can be changed
as well. The time of day is determined by your system clock.

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testWeather.
- Start testWeather.

Win32 Binary:
- Go to examples/testWeather/Release.
- Double-click testWeather.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testWeather: run 'make' from examples/testWeather.
- Enter examples/testWeather directory.
- Run 'testWeather'.

Controls
--------
F1-F5  -  Set visibility (F1:unlimited - F5:close)
1-5    -  Set cloud level (1:min - 5:max)
H      -  Bring up gui_fl
Esc    -  Exit

Left Mouse Button    -  Rotate View
Right Mouse Button   -  Translate View
Middle Mouse Button  -  Zoom View
