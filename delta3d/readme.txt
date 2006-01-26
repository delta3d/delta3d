########################################
# Delta3D: The Open-Source Game Engine #
########################################

Welcome to Delta3D ! We hope to get you up and running with Delta3D as
quickly as possible. If you only wish run pre-built applications and have
already used the Win32 installer, you are ready to rock. 

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

Here's what we know does works:

Win32 w/ Microsoft Visual Studio .NET 7.1
Win32 w/ Microsoft Visual C++ Toolkit 2003, .NET Framework SDK 1.1, & SCons
Linux w/ gcc3.4.x
Linux w/ gcc4.0.x

Note about Linux distrubutions: We test on Fedora Core 4 but have reports of 
Delta3D running on Ubuntu, SuSE, Mandriva, & Gentoo. Please contact us if you
are able to setup repositories for any Linux distribution to ease installation
for other users.

################
# Dependencies #
################

Check if you have the ext/ directory already in delta3d/. If you do not 
have it, please download the latest version of delta3d-dependencies for
Win32 or Fedora Core 4 from our SourceForge download site:
http://sourceforge.net/project/showfiles.php?group_id=113203&package_id=125151
and extract it into the Delta3D folder.

If you are not on Win32 or Fedore Core 4, you will need to either build the depedencies
yourself or track them down via your favorite package manager. So here's a handy list.

See the Depedencies page on http://www.delta3d.org for more info on version compatibility.

Required Libraries 
------------------
CAL3D
CppUnit
Crazy Eddie's GUI System
FLTK
Game Networking Engine
GDAL
InterSense Interface Libraries SDK
HawkNL
OpenAL
ALUT
Open Dynamics Engine
OpenThreads
OpenSceneGraph
Producer
PLIB
ReplicantBody
Xerces

Linux only: Xxf88vm, uuid, curses.

Optional Libraries
------------------
Qt 4.x - Needed for STAGE.
Boost - Needed for Python bindings only.
Python - Needed for Python script support.
RTI 1.3 - Needed for HLA libraries, utilities, & examples.

################
# Example Data #
################

In order to run our examples, you also need our sample data package. It can
also be found on SourceForge download site:
http://sourceforge.net/project/showfiles.php?group_id=113203&package_id=125206

#########################
# Environment Variables #
#########################

Delta3D requires environment variables at runtime (but not at compile-time).
The Windows installer will set these up for you, but if you use the zip
package or are on Linux you need to set them youself.

Win32
-----
Make sure the Delta3D environment variables are set:

DELTA_ROOT = c:\program files\delta3d  (or wherever Delta3D is installed)
DELTA_INC  = %DELTA_ROOT%\inc;%DELTA_ROOT%\ext\inc;%DELTA_ROOT%\ext\inc\CEGUI
DELTA_LIB  = %DELTA_ROOT%\lib;%DELTA_ROOT%\ext\lib
DELTA_DATA = %DELTA_ROOT%\data

And, of course, add  %DELTA_ROOT%\bin;%DELTA_ROOT%\ext\bin to your PATH.

Once set, these variables can be used in Visual Studio in the project properties
or in the global VC++ directories (Tools->Options->Projects) as shown below:

Include files : $(DELTA_INC) 
Library files : $(DELTA_LIB)

Linux
-----
DELTA_ROOT: path to your Delta3D installation
DELTA_DATA: $DELTA_ROOT/data
DELTA_INC:  $DELTA_ROOT/inc:$DELTA_ROOT/ext/inc:$DELTA_ROOT/ext/inc:$DELTA_ROOT/ext/inc/CEGUI
DELTA_LIB:  $DELTA_ROOT/lib:$DELTA_ROOT/ext/lib:$DELTA_ROOT/ext/lib/osgPlugins

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DELTA_LIB

########################################
# Platform-specific build instructions #
########################################

Win32 w/ Visual Studio .NET 7.1
-------------------------------

1. Open VisualStudio/delta.sln, build! This will generate the essential Delta3D
   libraries that can be built with our distributed dependencies.

2. The rest of the components can be build with their own solutions
   (dependencies that we do not distribute are listed in parentheses):
   
   VisualStudio/src/dtHLA/dthla.sln          : HLA library (RTI)
   VisualStudio/src/dtScript/dtscript.sln    : library to run python scripts (Python)
   VisualStudio/src/python/dtpython.sln      : python bindings (Python, Boost.Python)
   VisualStudio/src/python/dthlabindings.sln : python HLA bindings (Python, Boost.Python, RTI)
   VisualStudio/examples/*/*.sln             : individual examples
   VisualStudio/utilities/*/*.sln            : individual utilities
 
   Or just build EVERYTHING (make sure you have Python, BoostPython, RTI):
   VisualStudio/delta_all.sln  

   dtHLA
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLA module is compiled and tested
   against RTI-S 1.3_D11A although any RTI supporting spec 1.3 should work.

   To build the module dtHLA, example testHLA, and utility hlaStealthViewer, 
   you must add your RTI include and lib paths to the project settings or to
   your global search directories using :

   Tools->Options->Projects->VC++ Directories

   VS solutions are provided for these apps as:

   src/dtHLA/dthla.sln
   examples/testHLA/testHLA.sln
   utilities/hlaStealthViewer/hlaStealthViewer.sln

   dtPython
   --------  
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
      
   3. Unzip Boost. The directory will look something like "boost_1_33_1"
   
   4. Obtain the Boost-Jam executable from SourceForge (package 'boost-jam')
   
   5. Unzip Boost-Jam ('bjam.exe') and place it somewhere in your PATH or
      into your Boost directory
  
   6. Open a cmd shell and navigate to your Boost directory.
   
   7. Run 'bjam stage'. This will build the entire set of Boost libraries to
      a directory called 'boost_1_33_1\stage\lib'. Make sure to add this directory 
      to your PATH environment variable so Windows can find the Boost.Python DLLs 
      at runtime.
   
      Unfortunately, doing a specific Boost.Python build ends up with wrong
      filenames (probably a bug in their build system), so you must do a full
      build :(
   
   8. Open a solution contain the project files for the Delta3D Python bindings:
      -delta3d\VisualStudio\src\python\dtpython.sln or
      -delta3d\VisualStudio\delta_all.sln.
      
   9. Before building...
   
      -Select Tools->Options
      -Select the Projects folder
      -Select VC++ Directories
      -Change 'Show directories for:' to Include files
      -Add your Boost directory to the list
      -Change 'Show directories for:' to Library files
      -Add 'boost_1_33_1/stage/lib' (or whatever your proper path is)to the list
   
   10. Build!
   
   11. Make sure the .pyd libraries were built into delta3d\bin. If not, rinse and repeat.
   
   12. Set your PYTHONPATH environement variable contains the directory with the resulting 
       .pyd libraries (most likely delta3d/bin).

   STAGE
   -----
   If you want to build STAGE in with Microsoft's compiler (with SCons or VisualStudio),
   there's a few hoops you must jump through. The problem boils down to this: Trolltech 
   generously decided to release Qt 4 under an open-source license, but unfortunately 
   they only provided makefile support for MinGW. While we love MinGW, there is no support 
   for building Delta3D with it. 

   So we had to find a way to compile Qt with MSVC. Here's the solution:

   You'll need:
   -Visual Studio .NET 2003
   -Qt 4.x, the windows open-source release (http://www.trolltech.com)
   -Python (http://www.python.org/)
   -QtWin/Free patch for Qt 4.x (http://sourceforge.net/projects/qtwin/), 
    or from the Visual Studio dependencies release inside the ext/ directory)
   
   Building Qt 4.x Open Source Version with MSVC
   -----------------------------------------------
   1. Make sure all of the above software is installed.
   
   2. Extract the QtWin/Free patch inside your Qt directory.
      Overwrite any files with the ones from the patch.
      
   3. Follow instructions for the patch with its readme.
   
   4. After many many hours, it should be all compiled up and happy.
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

Win32 w/ SCons, Linux
---------------------

1. Download SCons from http://www.scons.org .

2. You will notice files called SConstruct and SConscript in the delta3d 
   subdirectories. These files are high-level scripts that tell SCons how 
   to build Delta3D.

   To build the Delta3D libraries from the root delta3d directory type:
    scons

   To build the examples:
    scons examples

   To build the utilities:
    scons utilities

   To build HLA-related libraries, examples, and utilities:
    scons hla

   To build the Python bindings and example:
    scons python 

   To install the shared libraries in the lib folder under the prefix
   you specified:
    scons install

   Use options include:
    -Q     - Quiet output.
    -j N   - Number of jobs to use, help for multiple processors.
    -c     - Clean out the previous build.
    --help - Display all options

    prefix=path - Path to in which to install Delta3D.
    mode=debug|release - 'debug' builds with debugging symbols.
                         'release' builds with optimizations enabled.
    no_warnings=1 - Turns off all compiler warnings.
    unit_tests=1 - Build unit tests.
    boost=path to your boost installation
    rti=path ro your RTI installation
    cpppath=additional include directories (comma delimited)
    libpath=additional library directories (comma delimited)

3. To build your own Delta3D apps in SCons, feel free to hack on scons_template
   as a template for how to use SCons outside our source tree.   

   dtPython
   --------
   The Delta3D Python bindings requires the following additional 
   packages: Python and Boost.Python. 

   Install Boost Python (Linux w/ bjam, see above for MSVC):
   - Obtain the Boost library from SourceForge (package 'boost'):
     http://sourceforge.net/projects/boost/
   - Obtain Boost Jam (package 'boost-jam').
   - Decompress the BoostJam archive and place the file 'bjam' in your PATH.
   - Set env. var PYTHON_ROOT to your your python installation (usually /usr or /usr/local)
   - Set env. var PYTHON_VERSION to the 2-part major python version (e.g. 2.3 or 2.4)
   - Go to boost_1_33_1/libs/python/build.
   - Run 'bjam -sTOOLS=gcc'.
   - Root up and 'bjam install'

   Build the Python bindings for Delta3D engine:
   - Go to the delta3d root directory.
   - If your installation of Boost is in a non-standard location,
     run scons with the 'boost=/path/to/boost' option, otherwise just build
     as normal.
    
   Set the PYTHONPATH environement variable to contain the var DELTA_LIB.

   dtHLA
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLA module is compiled and tested
   against RTI-S 1.3_D11A although any RTI should work.

   Once your have an RTI installed, run:

   'scons rti=/path/to/rti hla'

   and SCons will build dtHLA, testHLA, and hlaStealthViewer.
