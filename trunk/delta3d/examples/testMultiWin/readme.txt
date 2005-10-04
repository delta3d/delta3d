########################################
#                                      #
# Delta3D Examples: testMultiWin       #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil

Purpose
-------
To demonstrate how to implement a multi-Window, multi-Camera application.  This
will create two Windows, and three Cameras.  Window #1 has one Camera, while
Window #2 has two Cameras.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testMultiWin\testMultiWin.sln.
- Build testMultiWin.
- Start testMultiWin.

Win32 Binary:
- Go to bin.
- Double-click testMultiWin.exe.

Linux Source:
- To build only testMultiWin, from the root delta3d folder:
  scons bin/testMultiWin

Linux Binary:
- cd bin
- ./testMultiWin

Controls
--------
Esc  -  Exit
