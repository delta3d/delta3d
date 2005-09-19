########################################
#                                      #
# Delta3D Examples: testRecorder       #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the dtCore::Recorder.  It provides example code
for what is needed to record class instances.

Instructions
------------
Win32 Source: 
- Open VisualStudio/examples/testRecorder.sln.
- Build testRecorder.
- Start testRecorder.

Win32 Binary:
- Go to bin.
- Double-click testRecorder.exe.

Linux Source:
- To build all examples: Run 'scons examples' from delta3d/.
- To build only testRecorder: run 'scons bin/testRecorder' from delta3d/.
- Enter bin directory.
- Run 'testRecorder'.

Controls
--------
'R'  -  Start/Stop recording.
'F'  -  Write recording out to file.
'L'  -  Load recording from file.
'P'  -  Playback recording.
Esc  - Exit

Use arrow keys or hold left mouse button over corresponding areas of the 
window to move camera based on chosen motion model.

Fly Motion Model:
Up     -  Look up
Down   -  Look down
Left   -  Turn left
Right  -  Turn right
S      -  Move forward
W      -  Move backward
Left  Mouse Button - Look up/down, turn left/right
Right Mouse Button - Move forward/backward

Intended use:
Type 'R' to start recorder, fly around the world a little.  Type 'R' again
to stop recorder.  Type 'F' to dump the recording out to file.  Type 'L' to
load the recording from file.  Type 'P' to watch the recording.
