FireFighter
Demonstrates most of the functionality of Delta3D in a full-blown game setting.
The objective is to walk through the ship performing the required tasks.  If you
manage to complete all the tasks, you win!

To run, use the GameStart utility and supply the library name on the command line.  The
demo's working directory should be the delta3d root folder.

Usage:
"GameStart.exe fireFighter" or "GameStartD.exe fireFighter"

When the target reticule is over a usable item, a hand icon will appear.  Press
the 'f' key to "activate" that item (take, use, etc.).

Cycle the items in your inventory to use by pressing the '[' and ']' keys.  The
bar above the item's icon indicates which item is selected.

Use the currently selected inventory item by pressing the left mouse button.  The
bar under the item's icon indicates it's currently being used.

w,a,s,d keys control your movement.

Walkthrough (you cheater!) :
1) Acquire/don the fire suit
2) Acquire/turn on the SCBA (the breather mask)
3) Throw primary halon (big red bottle)
4) Throw the secondary halon (little red bottles)
5) Trip the fuel switch
6) Grab the hose (the one outside the engine room)
7) Open the hatch
8) Squirt water on the fire until it goes out
9) Rejoice in a successful mission

** Visual Studio **
If running via Visual Studio, edit the fireFighter project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: fireFighter
 - Debugging->Working Directory: <set to your Delta3D root folder>
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 
