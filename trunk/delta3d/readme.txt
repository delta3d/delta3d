########################################
# Delta3D: The Open-Source Game Engine #
########################################

Welcome to Delta3D ! We hope to get you up and running with Delta3D as
quickly as possible. If you only wish run pre-built applications and have
already used the Windows installer, you are ready to rock. 

Go ahead and try out the examples by running the executables found in
the bin folder.

There are also several useful utitilies also found in the delta3d/bin folder.
See the readme.txt in delta3d/utilities for more details.

If you are a developer you'll want to build from the source
which we provide with all our distributions. Please follow instructions
in the below instructions to build the source.

Thanks for using Delta3D! Please visit our webpage at http://www.delta3d.org
and don't hesistate to use the forums for any problems you may have. We'll do
our best to respond as quick as possible.

Sincerely,
The Delta3D Team

########################
# Building From Source #
########################

Here's what we know does work:

Microsoft Windows XP and Vista with Microsoft Visual Studio 2003-2008
Linux w/ gcc3.3.x
Linux w/ gcc3.4.x
Linux w/ gcc4.0.x
Linux w/ gcc4.1.x
Apple MacOS X 10.4 w/ the latest XCode tools

Note about Linux distrubutions: We test on Fedora Core 4 but have reports of 
Delta3D running on Ubuntu, SuSE, Mandriva, & Gentoo. Please contact us if you
are able to setup repositories for any Linux distribution to ease installation
for other users.

################
# Dependencies #
################

Delta3D makes use of many different external dependencies; some are
required and some are optional.

If you are on Windows and are using a supported version of 
Visual Studio, we have prepared pre-compiled dependency pacakges for you.
These will live as a folder called 'ext' that should reside inside
your Delta3D directory. 

Please see the latest information on the External Dependencies at:
http://delta3d.wiki.sourceforge.net/Dependencies.


#########################
# Environment Variables #
#########################

Delta3D requires environment variables at runtime (but not at compile-time).
The Windows installer will set these up for you, but if you use the zip
package or are on Linux you need to set them youself.

Microsoft Windows
--------------------
Make sure the Delta3D environment variables are set:

DELTA_ROOT = "C:\Program Files\delta3d"  (or wherever Delta3D is installed)
DELTA_INC = %DELTA_ROOT%\inc;%DELTA_ROOT%\ext\inc;%DELTA_ROOT%\ext\inc\CEGUI
DELTA_LIB = %DELTA_ROOT%\lib;%DELTA_ROOT%\ext\lib
DELTA_DATA = %DELTA_ROOT%\data
DELTA_BIN = %DELTA_ROOT%\bin;%DELTA_ROOT%\ext\bin

By adding DELTA_BIN to your PATH environment, all Delta3D applications
will find the Delta3D .dll files.

Once set, these variables can be used in Visual Studio in the project properties
or in the global VC++ directories (Tools->Options->Projects) as shown below:

Include files: $(DELTA_INC)
Library files: $(DELTA_LIB)

Linux
-----
DELTA_ROOT: path to your Delta3D installation
DELTA_DATA: $DELTA_ROOT/data
DELTA_INC: $DELTA_ROOT/inc:$DELTA_ROOT/ext/inc:$DELTA_ROOT/ext/inc:$DELTA_ROOT/ext/inc/CEGUI
DELTA_LIB: $DELTA_ROOT/lib:$DELTA_ROOT/ext/lib:$DELTA_ROOT/ext/lib/osgPlugins

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DELTA_LIB

########################################
# Build instructions                   #
########################################

Building with CMake
-------------------------------

1. Make sure you have your depedencies setup (see above).

2. Run CMake.exe (Windows) or ccmake (Linux), supplying the %DELTA_ROOT%/CMakelists.txt file.

3. Select the Build options for the projects you wish to generate. Some
   build options enable other options and require additional external
   dependencies, some of which might not be distributed.
   
4. Click the Configure button and resolve any missing dependencies.  Continue
   to Configure and set options.

5. Once the CMake Generate button is enabled, you're good to go.  Click on 
   Generate and the project files will be generated
   
6. See the online tutorials for more building information.   


   dtHLAGM
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLAGM module is compiled and tested
   against RTI-S 1.3_D18 although any RTI supporting spec 1.3 should work.


   The Python Bindings
   -------------------
   Building the Delta3D Python bindings requires the following additional 
   packages: Python and Boost.Python. 

   1. First let's install Python from: http://www.python.org/
   
   2. Next, check if the following environement variables are set:
      PYTHON_ROOT     : directory of Python installation
      PYTHON_LIB_PATH : %PYTHON_ROOT%/libs
      PYTHON_VERSION  : version of your Python installation

   2. Ok, that was easy :) Now the hard part: installing Boost. Feel
      free to cross-reference with their "Getting Started" guide:
      http://www.boost.org/more/getting_started.html   
      
      Obtain the Boost library from SourceForge (package 'boost'):
      http://sourceforge.net/projects/boost/
      
   3. Unzip Boost. The directory will look something like "boost_1_34_1"
   
   4. Obtain the Boost-Jam executable from SourceForge (package 'boost-jam')
   
   5. Unzip Boost-Jam ('bjam.exe') and place it somewhere in your PATH or
      into your Boost directory
  
   6. Open a cmd shell and navigate to your Boost directory.
   
   7. Run 'bjam stage --with-python' and 'bjam stage --with-python debug'. This will build the Release/Debug
      Boost.Python set of Boost libraries to a directory something like "boost*/stage/lib". Make sure to add this directory 
      to your PATH environment variable so Windows can find the Boost.Python DLLs 
      at runtime.
         
   8. If you are using the Windows installer or have the dependency package unzipped
      make sure to remove the Boost.Python DLLs contained in delta3d\ext\bin before
      going any further. Otherwise, they will conflict with the DLL you just built.
   
   9. Ensure CMake has the valid Boost include and library paths
   
   10. Build!
   
   11. Ensure the generated .pyd files are in the /bin folder under your CMake build folder.
   
   12. Set your PYTHONPATH environment variable contains the directory with the resulting 
       .pyd libraries.

   STAGE
   -----
   You'll need:
   -Qt 4.x, the Windows or Linux open-source release (http://www.trolltech.com)
   
   
   Building Qt 4.x Open Source Version with MSVC
   -----------------------------------------------
    1. To configure Qt, just use the Visual Studio Command prompt, change
        directory to where Qt is, and type "configure -platform win32-msvc2005" (or
        whatever compiler you have installed).  To see the list of supported 
        compilers, see the folder "mkspecs" in the Qt installation.
        
     2. Once that is finished type "nmake" to start the compiling.  If you just want
          to build the libraries (much faster), type "nmake sub-src".
   
   
   Building STAGE
   -------------------------------------
   
   1. IMPORTANT: If you have installed the Windows dependency package or
      used the full-on installer, you must remove the Qt DLLs that are 
      shipped inside ext\bin. These were placed there so users could
      use the STAGE binary without rebuilding Qt. However, if you are 
      reading this section it means you _are_ rebuilding Qt in order
      to rebuild STAGE, so the old DLLs will conflict.
   
   2. If all the requirements are met, CMake should generate the STAGE project.
   
   3. Build!.

   4. The STAGE.exe file should now reside in the /bin folder of your CMake 
      build folder.  Double-click to run!
