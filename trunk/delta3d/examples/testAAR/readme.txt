########################################
#                                      #
# Delta3D Examples: testLogger         #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtGame, dtDAL

Purpose
-------
To demonstrate the logging and playback components of the GameManager.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testLogger\testLogger.sln.
- Build testLogger.
- Start testLogger.

Win32 Binary:
- Go to bin.
- Double-click testLogger.exe.

Linux Source:
- To build only testLogger, from the root delta3d folder:
  scons bin/testLogger

Linux Binary:
- cd bin
- ./testLogger

Controls
--------
Space  - Display status of LogController
B      - Drop an object
C      - Change camera mode (follow player or free camera)
1      - Change to idle state (do this before replaying)
2      - Start logging
3      - Replay logged events
[      - Decreate simulation speed
]      - Increase simulation speed
0      - Reset simulation speed
Return - Display next statistics type
P      - Pause
Esc    - Exit

Free Camera Mode:
Left   Mouse Button - Rotate camera
Right  Mouse Button - Translate camera
Middle Mouse Button - Zoom camera

Follow Player Mode:
W - Move forward
S - Move backward
A - Turn left
D - Turn right
