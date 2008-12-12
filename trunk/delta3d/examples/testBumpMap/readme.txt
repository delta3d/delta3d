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

Linux Binary:
- cd bin
- ./testBumpMap

Controls
--------
Space - Toggle the geometry that is rendered
Esc   - Exit
'1'   - Render lit with bump
'2'   - Render base texture
'3'   - Render the normal texture
'4'   - Render the diffuse color
'5'   - Render the specular color
'6'   - Render the full lighting contribution

Orbit Motion Model:
Left  Mouse Button  - Look up/down, turn left/right
Middle Mouse Button - Move closer/farther
Right Mouse Button  - Move forward/backward
