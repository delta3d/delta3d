##############################
#                            #
# Delta3D STAGE :            #
# Simulation, Training, And  #
# Game Editor                #
#                            #
##############################

So, you want to build the Editor?  This document will hopefully explain
the process to get it compiled and running on your machine.

However, if you want to build it in VisualStudio, there's a few hoops you
must jump through. The problem boils down to this: Trolltech generously decided 
to release Qt 4.0.1 under an open-source license, but unfortunately they only provided
makefile support for MinGW. While we love MinGW, there is no support for 
building Delta3D with it. 

So we had to find a way to compile Qt with MSVC. Here's the solution:

You'll need:
-Visual Studio .Net 2003
-Qt 4.0.1, the windows open-source release 
 ftp://ftp.trolltech.com/qt/source/qt-win-opensource-desktop-4.0.1.zip
-Python
 http://www.python.org/ftp/python/2.4.1/python-2.4.1.msi
-qt4.0.1_msvc_patch.zip 
 (get this from the Visual Studio dependencies release inside the ext/ directory)

Building Qt 4.0.1 Open Source Version with MSVC
--------------------------------------------

1. Make sure all of the above software is installed.
   Qt *must* be installed to C:\Qt\4.0.1
   
2. Add the following environment variables:

   QTDIR=C:\Qt\4.0.1
   QMAKESPEC=win32-msvc.net
   Add C:\Qt\4.0.1\bin to your PATH if they aren't there already.
   
3. Extract qt4.0.1_msvc_patch.zip inside the C:\Qt\4.0.1 directory.
   Overwrite any files with the ones from the patch.
   
4. Open a Command window by clicking Start->Run..., and typing 'cmd'

5. The following commands should be run from the command line (without quotes,
   and replace the VisualStudio path with your appropriate path):
   
   'cd %QTDIR%'
   'qtvars.bat' (ignore the mention of MinGW in this output of this command)
   'C:\Program File\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat'
   'qmake'
   'nmake'

6. After many many hours, it should be all compiled up and happy.
   Qt is now built with MSVC!
   
   
Building the Delta3D Editor with MSVC
-------------------------------------
1. Ok, onto building the editor. Open:
   delta3d\VisualStudio\utilities\editor\editor.sln
   
2. Now we are going to make sure VisualStudio can find python.exe:
   
   Select Tools->Options
   Select the Projects folder
   Select VC++ Directories
   Change 'Show directories for:' to Executable files
   Add 'C:\Python24' to the list of directories (or whatever you proper path is)
   
3. Build the entire solution.

4. The STAGE.exe file should now reside in delta3d/bin.  Double-click to run!
