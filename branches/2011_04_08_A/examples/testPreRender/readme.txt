########################################
#                                      #
# Delta3D Examples: testPreRender      #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To provide an example of how to do render to texture.

Definition
----------
Rendering to a texture is a useful process for many simulations.  It allows us generate an image (color, depth, stencil)
before we render to the screen.  Some techniques that make use of the this include but are in now way limited to
post processing effects, reflections, and shadowing.


Controls
--------
Esc   - Exit

Orbit Motion Model:
Left  Mouse Button  - Look up/down, turn left/right
Middle Mouse Button - Move closer/farther
Right Mouse Button  - Move forward/backward
