########################################
#                                      #
# Delta3D Examples: testLights         #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To demonstrate the Delta3D lighting system. Our implementation is a thin
object-oriented wrap of osg::Light and osg::LightSource, which is in turn a
wrap of OpenGL vertex lighting. This example simply creates 3 lights in a 
scene (not including the default scene light, which has been turned off 
here) and demos their functionality. Light 1 is a spotlight rotating around
the warehouse with continuously changing color. Light 2 is a yellow 
positional light. The sphere is a Transformable child of Light 2 (i.e. when 
Light 2 moves, the sphere moves). Light 3 is an infinite light (same as the 
OpenGL directional light) and has the appearance of being located at an 
infinite distance from the scene and therefore only it's direction and color
can change (and change they do).


Controls
--------
1    -  Toggle Global SpotLight on/off (Light 1)
2    -  Toggle PositionalLight on/off (Light 2)
3    -  Toggle Local/Global on PositionalLight (Light 2)
4    -  Toggle Global InfiniteLight on/off (Light 3)
Esc  -  Exit

Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera
