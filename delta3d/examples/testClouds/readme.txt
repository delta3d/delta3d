########################################
#                                      #
# Delta3D Examples: testClouds         #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the CloudPlane and CloudDome class.  The CloudPlane is a planar-sytle cloud
while the CloudDome is spherical. CloudDome is implemented using the OpenGL Shading
Language and requires a graphics card which supports GLSL.

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testClouds.
- Start testClouds.

Win32 Binary:
- Go to examples/testClouds/Release.
- Double-click testClouds.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testClouds: run 'make' from examples/testClouds.
- Enter examples/testClouds directory.
- Run 'testClouds'.

Controls
--------
F1-F5  -  Set visibility (F1:unlimited - F5:close)
H      -  Bring up gui_fl
P      -  Turn off cloud dome
D      -  Turn on cloud dome
+|-    -  Increase|decrease number of cloud layers
Esc    -  Exit

Left Mouse Button    -  Rotate View
Right Mouse Button   -  Translate View
Middle Mouse Button  -  Zoom View

