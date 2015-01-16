Tank Target Tutorial
This tutorial will introduce a variety of concepts such as Game Actors, 
Game Components, and Game Messages.  It will explain everything you
 need to know and do to build a completely functional 
 Delta3D GameManager-based application.

To run, use the GameStart utility and supply the library name on the command line.
Ensure the "working directory" is set to %DELTA_ROOT%/demos/TankTargetTutorial or 
else the data files will not be found.

Usage:
"GameStart.exe TutorialLibrary" or "GameStartD.exe TutorialLibrary"



** Visual Studio **
If running via Visual Studio, edit the testAAR project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: TutorialLibrary
 - Debugging->Working Directory: %DELTA_ROOT%/demos/TankTargetTutorial
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 
