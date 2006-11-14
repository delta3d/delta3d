########################################
#                                      #
# Delta3D Examples: testPhysics        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To demonstrate the physics package implemented in Delta3D.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testPhysics\testPhysics.sln.
- Build testPhysics.
- Start testPhysics.

Win32 Binary:
- Go to bin.
- Double-click testPhysics.exe.

Linux Source:
- To build only testPhysics, from the root delta3d folder:
  scons bin/testPhysics

Linux Binary:
- cd bin
- ./testPhysics

Controls
--------
B  -  Drop crate (box)
C  -  Drop radioactive drum (cylinder)
S  -  Drop a happy sphere (sphere)
P  -  Pause
Esc - Exit

Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera
