FireFighter
Demonstrates most of the functionality of Delta3D in a full-blown game setting.
The objective is to walk through the ship performing the required tasks.  If you
manage to complete all the tasks, you win!

To run, use the GameStart utility and supply the library name on the command line.

Usage:
"GameStart.exe fireFighter" or "GameStartD.exe fireFighter"



** Visual Studio **
If running via Visual Studio, edit the fireFighter project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: fireFighter
 - Debugging->Working Directory: <set to your Delta3D root folder>
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 
