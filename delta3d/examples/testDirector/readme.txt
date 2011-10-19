########################################
#                                      #
# Delta3D Examples: testDirector       #
#                                      #
########################################

Uses modules: dtABC, dtActors, dtCore, dtDAL, dtDirector, dtGame, dtUtil

Purpose
-------
To provide an example of how to use the Director Graph to provide
level scripting for your application.

Examples
--------
- A basic swinging door.
- A more complex swinging door that swings both ways.
- A sliding door.
- A fireplace that lights when you walk near it.


Controls
--------
W,S   - Move foward and back.
A,D   - Strafe left and right.
Space - Jump
Esc   - Exit

Running
-------
To run, use the GameStart utility and supply the library name on the command line.

Usage:
"GameStart.exe testDirector" or "GameStartD.exe testDirector"



** Visual Studio **
If running via Visual Studio, edit the testDirector project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: testDirector
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 