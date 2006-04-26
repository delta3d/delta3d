########################################
#                                      #
# Delta3D Examples: testAAR            #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtGame, dtDAL, testGameActorLibrary, testActorLibrary

Purpose
-------
To demonstrate the logging and playback components of the GameManager.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testAAR\testAAR.sln.
- Build testAAR.
- Start testAAR.

Win32 Binary:
- Go to bin.
- Double-click testAAR.exe.

Linux Source:
- To build only testAAR, from the root delta3d folder:
  scons bin/testAAR

Linux Binary:
- cd bin
- ./testAAR

Controls
--------
Space  - Display status of LogController
B      - Drop an object
T      - Print tasks
1      - Change to idle state (do this before replaying)
2      - Start logging
3      - Replay logged events
[      - Decreate simulation speed
]      - Increase simulation speed
0      - Reset simulation speed to realtime
Return - Display next statistics type
P      - Pause
Esc    - Exit

Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera

W - Move camera forward
S - Move camera backward
A - Turn camera left
D - Turn camera right

I - Move camera forward
K - Move camera backward
J - Turn camera left
L - Turn camera right
