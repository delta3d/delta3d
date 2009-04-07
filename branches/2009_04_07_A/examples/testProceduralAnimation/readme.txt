testProceduralAnimation
Demonstrates the procedural capabilities of the animation system funcionality for acheiving gaze control and gun aiming.

To run, use the GameStart utility and supply the library name on the command line.

Usage:
"GameStart.exe testProceduralAnimation" or "GameStartD.exe testProceduralAnimation" and optionally "--performanceTest"

Normal mode:
  Use WASDF and the mouse to walk across the terrain while several character fixate upon your positon.  Characters will be enganed
  in either following you with their gaze or aiming at you with their weapon.
  
Performance mode (--performanceTest)
  Use WASDF and the mouse to fly around a grid of procedurally animated characters.  Notice how many can be displayed and procedurally
  animated while maintaining a high framerate.  The procedural system is very efficient and does not significantly burden performance.
  
Both Modes:

 '~'      Toggle statistics
 'Escape' Quit the application.
 
 For further information on system details and usage, see the paper presented at I/ITSEC 2008 here: 
 http://delta3d.wiki.sourceforge.net/Tutorial_Procedural_Character_Animation


