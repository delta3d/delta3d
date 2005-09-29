########################################
#                                      #
# Delta3D Examples: testBumpMap        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To provide an example of how to do dot3 bumpmapping
with GLSL.

Instructions
------------
Win32 Source: 
- Open VisualStudio/examples/testBumpMap\testBumpMap.sln.
- Build testBumpMap.
- Start testBumpMap.

Win32 Binary:
- Go to bin.
- Double-click testBumpMap.exe.

Linux Source:
- To build only testBumpMap, from the root delta3d folder:
  scons bin/testBumpMap

Linux Binary:
- cd bin
- ./testBumpMap

Controls
--------
Space - Wireframe
Esc   - Exit

Fly Motion Model:
Up     -  Look up
Down   -  Look down
Left   -  Turn left
Right  -  Turn right
S      -  Move forward
W      -  Move backward
Left  Mouse Button - Look up/down, turn left/right
Right Mouse Button - Move forward/backward
