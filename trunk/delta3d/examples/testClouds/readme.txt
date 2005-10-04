########################################
#                                      #
# Delta3D Examples: testClouds         #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To demonstrate the CloudPlane and CloudDome class.  The CloudPlane is a planar-sytle cloud
while the CloudDome is spherical. CloudDome is implemented using the OpenGL Shading
Language and requires a graphics card which supports GLSL.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testClouds\testClouds.sln.
- Build testClouds.
- Start testClouds.

Win32 Binary:
- Go to bin.
- Double-click testClouds.exe.

Linux Source:
- To build only testClouds, from the root delta3d folder:
  scons bin/testClouds

Linux Binary:
- cd bin
- ./testClouds

Controls
--------
F1-F5  -  Set visibility (F1:unlimited - F5:close)
H      -  Bring up the Inspector
P      -  Turn off cloud dome
D      -  Turn on cloud dome
+|-    -  Increase|decrease number of cloud layers
Esc    -  Exit

Left Mouse Button    -  Rotate View
Right Mouse Button   -  Translate View
Middle Mouse Button  -  Zoom View

