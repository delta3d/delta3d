testGameActorComponents
Demonstrates the ActorComponent system by creating a new type of actor that
has a ActorComponent added to it

To run, use the GameStart utility and supply the library name on the command line.

Usage:
"GameStart.exe testGameActorComponents" or "GameStartD.exe testGameActorComponents"



** Visual Studio **
If running via Visual Studio, edit the testGameActorComponents project settings:
 - Debugging->Command: GameStart.exe (or "GameStartD.exe" for debug).
 - Debugging->Command Arguments: testGameActorComponents
 
Note: Make sure your PATH environment is set to %DELTA_ROOT%/bin so MSVC can 
      find the GameStart executable. 
