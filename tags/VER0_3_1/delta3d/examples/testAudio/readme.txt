########################################
#                                      #
# Delta3D Examples: testAudio          #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtAudio

Purpose
-------
To demonstrate advanced functionality of the dtAudio module, specfically
effect binding, panning, dopler effects, gain, pitch changing, and looping. 

Note: When a sound is played on Linux, the application hangs waiting for a
      message to be received. This is a known bug.

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testAudio.
- Start testAudio.

Win32 Binary:
- Go to examples/testAudio/Release.
- Double-click testAudio.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testAudio: run 'make' from examples/testAudio.
- Enter examples/testAudio directory.
- Run 'testAudio'.

Controls
--------
A      -  Plays a sound
S      -  Plays a sound bound to an effect
D      -  Plays a sound with panning
F      -  Plays a sound with panning and dopler
0-9    -  Sets gain
+|-    -  Raise|lower pitch
L      -  Set|unset all sounds looping
Space  -  Stops all sounds
Enter  -  Rewinds all sounds
Pause  -  Pause|unpause all sounds
Esc    -  Exit

Left Mouse Button    -  Rotate View
Right Mouse Button   -  Translate View
Middle Mouse Button  -  Zoom View
