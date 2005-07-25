########################################
#                                      #
# Delta3D Examples: testWin            #
#                                      #
########################################

NOTE! This example is broken on Fedora Core 4. There is some problem
using glGUI with FC4 or maybe gcc4. Don't worry, this is deprecated
anyways! Look for the CEGUI version soon.

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the ChangeScreenResolution and SetFullScreenMode functions
from the DeltaWin class.

Linux Notes:
- A call to FullScreen and a subsequent call to ChangeScreenResolution 
  (and vice versa) will not yield expected results. Producer uses the 
  virtual screen size to update its values for the screen width and height
  (which is a constant value on X11, your highest possible resolution), so 
  changing the resolution leaves the Producer values out of sync with the 
  real ones. 
- Also, changing the position iterferes with the window border.
  Setting the position will work if the border is off, so hitting
  Set, then Set again works (albeit with no window border). You can
  reset the border by toggle fullScreen. These are outstanding issues
  from Producer interacting with flaky X11 window managers.

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




