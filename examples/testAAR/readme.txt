TestAAR
Demonstrates the After Action Review capabilities of Delta3D.

To run, use the GameStart utility and supply the library name on the command line.

Usage:
"GameStart.exe testAAR" or "GameStartD.exe testAAR"



** Visual Studio **
If running via Visual Studio, edit the testAAR project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: testAAR
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 
