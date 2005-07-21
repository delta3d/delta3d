##############################
#                            #
# Delta3D STAGE :            #
# Simulation, Training, And  #
# Game Editor                #
#                            #
##############################

Alright ladies and gentlemen, we've been promising this editor for quite some
time now, and here it is!

However, if you want to build it in VisualStudio, there's a few hoops you
must jump through. Luckily, not all of the hoops on fire. The problem
boils down to this: Trolltech generously decided to release Qt 4.0.0 under
an open-source license (THANK YOU!!!), but unfortunately they only provided
makefile support for MinGW. While we love MinGW, there is no support for 
building Delta3D with it (yet ;). So we had to find a way to compile Qt
with MSVC. Here's the solution....

You’ll need:
-Visual Studio .Net 2003
-Qt 4.0.0, the windows open-source release 
 http://www.trolltech.com/download/qt/windows.html
-Python
 http://www.python.org
-qt4_msvc_patch.zip

Here we go...

1. Make sure all of the above software is installed.
   Qt must be installed to C:\Qt\4.0.0
   
2. Add the following environment variables:

   QTDIR=C:\Qt\4.0.0
   QMAKESPEC=win32-msvc.net
   Add C:\Qt\4.0.0\bin to your PATH if they aren't there already.
   
3.	Extract qt4_msvc_patch.zip inside the C:\Qt\4.0.0 directory.
   Overwrite any files with the ones from the patch.
   
4.	Open a Command window by clicking Start->Run..., and typing 'cmd'

5.	The following commands should be run from the command line (replace the 
   VisualStudio path with your appropriate path):
   
	cd %QTDIR
   qtvars.bat
   C:\Program File\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat
	qmake
   nmake

6. Woohoo! Qt is now built with MSVC :)

7. Ok, onto building the editor. Open:
   delta3d\VisualStudio\utilities\editor\editor.sln
   
8. Now we are going to make sure VisualStudio can find pytohn.exe:
   
   Select Tools->Options
   Select the Projects folder
   Select VC++ Directories
   Change 'Show directories for:' to Executable files
   Add 'C:\Python24' to the list of directories (or whatever you proper path is)
   
9. Build the sucker!

10. Make some damn fine maps.
