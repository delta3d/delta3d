########################################
#                                      #
# Delta3D Examples: testSound          #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtAudio

Purpose
-------
To demonstrate basic functionality of the dtAudio module.

Note: When a sound is played on Linux, the application hangs waiting for a
      message to be received. This is a known bug.

Instructions
------------
Win32 Source: 
- Open \VisualStudio\examples\testSound\testSound.sln.
- Build testSound.
- Start testSound.

Win32 Binary:
- Go to examples/testSound/Release.
- Double-click testSound.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testSound: run 'make' from examples/testSound.
- Enter examples/testSound directory.
- Run 'testSound'.

Controls
--------
Esc           - Exit
Space         - Generate multiple sounds
Any other key - Play a single sound


