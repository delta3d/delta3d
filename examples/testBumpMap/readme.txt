########################################
#                                      #
# Delta3D Examples: testBumpMap        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To provide an example of how to do dot3 normal mapping
with GLSL.

Definition
----------
A normal map provides the per-pixel normals in texture by 
storing vectors instead of colors, so RGB is really (X,Y,Z).
The normals define the pertubations of the surface.  Since the
normals are relative to the surface they are on, the light vector
must be transformed to this space (tangent space) before doing the 
lighting calculation.  


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
