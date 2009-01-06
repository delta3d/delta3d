########################################
#                                      #
# Delta3D Examples: testMotionModels   #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtGUI

Purpose
-------
To demonstrate the four motion models supplied with Delta3D. The walk motion
model simulates walking or driving. The fly motion model simulates the control
of an object in flight. The UFO motion model allows the camera to easily move 
along a horizontal plane easily, while additional controls let camera move 
up/down and turn left/right. The orbit motion model simulates the standard
controls to manipulate a 3D scene for viewing from arbitrary point (i.e.
translate, rotation, and zoom).


Controls
--------
1    -  Choose "Walk" motion model
2    -  Choose "Fly" motion model
3    -  Choose "UFO" motion model
4    -  Choose "Orbit" motion model
Esc  - Exit

Clicking on the HUD will switch motion models as well.

Use arrow keys or hold left mouse button over corresponding areas of the 
window to move camera based on chosen motion model.

Walk Motion Model:
Up     -  Move forward
Down   -  Move backward
Left   -  Turn left
Right  -  Turn right
A      -  Strafe left
D      -  Strafe right
Left  Mouse Button - Move forward/backward, turn left/right
Right Mouse Button - Strafe left/right

Fly Motion Model:
Up     -  Look up
Down   -  Look down
Left   -  Turn left
Right  -  Turn right
S      -  Move forward
W      -  Move backward
Left  Mouse Button - Look up/down, turn left/right
Right Mouse Button - Move forward/backward

UFO Motion Model:
Up     -  Move forward
Down   -  Move backward
Left   -  Strafe left
Right  -  Strafe right
A      -  Turn left
D      -  Turn right
S      -  Move up
W      -  Move down
Left  Mouse Button - Move forward/backward, strafe left/right
Right Mouse Button - Move up/down, turn left/right

Orbit Motion Model:
Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera

FPS Motion Model:
Passive Mouse Motion - Rotate camera
W                    - Move forward
S                    - Move backward
A                    - Move left
D                    - Move right