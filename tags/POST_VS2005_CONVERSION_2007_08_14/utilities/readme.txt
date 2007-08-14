##############################
#                            #
# Delta3D: Utilities         #
#                            #
##############################

A collection of useful utilities for a Delta3D user/developer. Please follow
instructions in the main readme.txt for building from source. Otherwise,
use the .exe in the Release folders.


editor
------
This is STAGE: the Simluation, Traning, And Game Editor.
Check out the main readme.txt in the Delta3D root directory  for
instructions on how to build it on Windows.

For linux, simply have QTDIR set to your Qt installation and do a:
'scons .', which should build everything Delta3D has to offer.

GameStart
---------
GameStart is a handy little utility that let's you build your game's
starting logic and setup into a game actor library. Simply make a class
that derives from dtGame::GameEntryPoint and export it in your library
(let's call the libary MyGameActorLibrary), overriding the pure virtual 
function OnStartup. Then you can use the GameStart executable like this:
   GameStart.exe MyGameActorLibrary

hlaStealthViewer
----------------
An simple tool that plots the location of HLA entities on a globe. Use this
with testHLA to verify the dtHLA module is working. Note: The RTI implementation
of HLA is not distrubted with Delta3D. Please see the main readme.txt in the
Delta3d root directory for more information.

psEditor
--------
A Particle System editor. Use this tool to create OSG particle effects in
real-time. 

viewer
------
Opens and views any OSG-compatible model.
