########################################
#                                      #
# Delta3D Examples: testCharacters     #
#                                      #
########################################

Uses modules: dtCore, dtChar, dtABC, dtUtil

Purpose
-------
To demonstrate basic functionality of the ReplicantBody/Cal3D character
animation toolkit. Two avatars are used and the user controls one while
the second follows the user.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testCharacters\testCharacters.sln.
- Build testCharacters.
- Start testCharacters.

Win32 Binary:
- Go to bin.
- Double-click testCharacters.exe.

Linux Source:
- To build only testCharacters, from the root delta3d folder:
  scons bin/testCharacters
  
Linux Binary:
- cd bin
- ./testCharacters

Controls
--------
Up     -  Walk character forward
Left   -  Turn character left
Right  -  Turn character right
Shift  -  Hold down to run
P      -  Pause
Esc    -  Exit

Left Mouse Button    -  Rotate View
Right Mouse Button   -  Translate View
Middle Mouse Button  -  Zoom View
