########################################
#                                      #
# Delta3D Examples: testWin            #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the ChangeScreenResolution and SetFullScreenMode functions
from the DeltaWin class.

Note: A call to FullScreen and a subsequent call to ChangeScreenResolution 
      (and vice versa) will not yield expected results. Producer does not
      update its values for the screen width and height, so changing the
      resolution leaves the Producer values out of sync with the real ones.

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testWin.
- Start testWin.

Win32 Binary:
- Go to examples/testWin/Release.
- Double-click testWin.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testWin: run 'make' from examples/testWin.
- Enter examples/testWin directory.
- Run 'testWin'.




