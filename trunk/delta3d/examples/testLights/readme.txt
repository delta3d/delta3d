########################################
#                                      #
# Delta3D Examples: testLights         #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the Delta3D lighting system. Our implementation is a thin
object-oriented wrap of osg::Light and osg::LightSource, which is in turn a
wrap of OpenGL vertex lighting. This example simply creates 4 lights in a 
scene (not including the default scene light, which has been turned off 
here) and demos their functionality. Light 1 is a spotlight rotating around
the warehouse with continuously changing color. Light 2 is a yellow local
positional light and it only affects the sphere. Light 3 is also yellow but
is a global positional light and affects the scene. The sphere is a 
Transformable child of Light 3 (i.e. when Light 3 moves, the sphere moves).
Light 4 is an infinite light (same as the OpenGL directional light) and has
the appearance of being located at an infinite distance from the scene and
therefore only it's direction and color can change (and change they do).

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testLights.
- Start testLights.

Win32 Binary:
- Go to examples/testLights/Release.
- Double-click testLights.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testLights: run 'make' from examples/testLights.
- Enter examples/testLights directory.
- Run 'testLights'.

Controls
--------
1    -  Toggle Global SpotLight
2    -  Toggle Local PositionalLight
3    -  Toggle Global PositionalLight
4    -  Toggle Global InfiniteLight
Esc  -  Exit

Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera