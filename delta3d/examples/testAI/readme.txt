########################################
#                                      #
# Delta3D Examples: testAI             #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtAI, dtDAL, dtActors, dtChar

Purpose
-------
To provide an example of how to use dtAI::AStar to move a dtChar::Character
between waypoints.


Instructions
------------
Win32 Source: 
- Open VisualStudio/examples/testAI/testAI.sln.
- Build testAI.
- Start testAI.

Win32 Binary:
- Go to bin.
- Double-click testAI.exe.

Linux Source:
- To build only testAI, from the root delta3d folder:
  scons bin/testAI

Linux Binary:
- cd bin
- ./testAI

Controls
--------
N     - Toggle NavMesh rendering
Space - Toggle overhead view
Esc   - Exit