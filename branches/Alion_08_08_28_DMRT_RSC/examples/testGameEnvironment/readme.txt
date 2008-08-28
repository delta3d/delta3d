#########################################
#                                       #
# Delta3D Examples: testGameEnvironment #
#                                       #
#########################################

Uses modules: dtUtil, dtCore, dtABC, dtDAL, dtActors, dtGame

Purpose
-------
To demonstrate how to manipulate weather and environmental effects
using the GameManager and EnvironmentActors.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testGameEnvironment\testGameEnvironment.sln.
- Build testGameEnvironment.
- Start testGameEnvironment.

Win32 Binary:
- Go to bin.
- Double-click testGameEnvironment.exe.

Linux Source:
- To build only testGameEnvironment, from the root delta3d folder:
  scons bin/testGameEnvironment

Linux Binary:
- cd bin
- ./testGameEnvironment

Controls
--------
1     - Toggle cloud planes
2     - Toggle fog
3     - Set close visibility
4     - Set moderate visibility
5     - Set unlimited visibility
6     - Toggle rainy and fair weather
7     - Toggle winter and summer seasons
8     - Toggle severe wind
Space - Toggle the EnvironmentActor itself
Esc   - Exit
