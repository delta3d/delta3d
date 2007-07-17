Animation Viewer

Simple utility used to load a definition of an animated character and display some
user interface controls to manipulate the character.

Requires that QT4.3.x is installed.

----------
To Build
----------

MSVC: Double-click the VisualStudio/utilities/AnimationViewer/BuildProject.bat file to 
generate the .vcproj file.  Then load the AnimationViewer.sln into MSVC and build as
either Debug or Release.

GCC: No project fils are currently created for Linux machines.  See 
VisualStudio/utilities/AnimationViewer/AnimationViewer.pro for an example.

---------
To Run
---------

Double-click $(DELTA_ROOT)/bin/AnimationViewer.exe to launch the application. Use
File->Open to browse to a previously defined character definition file (.xml).

Each row in the UI represents one of the defined animations.  You can enable/disable
each animation by selecting the check box for that row.  The Loop (L) weights
and timing can be adjusted as well as the Action (A) weights and times.  

To perform an animation as an Action, double-click the animation ID.
