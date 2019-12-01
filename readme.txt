########################################
# Delta3D: The Open-Source Game Engine #
########################################

Welcome to Delta3D! We hope to get you up and running with Delta3D as
quickly as possible. If you only wish run pre-built applications and have
already used the Windows installer, you are ready to rock. 

Go ahead and try out the examples by running the executables found in
the build/bin folder.

There are also several useful utitilies also found in the build/bin folder.

Thanks for using Delta3D! Please visit our webpage at http://www.delta3d.org
and don't hesistate to use the forums for any problems you may have. We'll do
our best to respond as quick as possible.

Sincerely,
The Delta3D Team


########################
# Building From Source #
########################

Delta3D uses CMake (http://www.cmake.org) to generate projects for the 
libraries, examples, and utilities.  See 
http://delta3d.org/deltawiki/index.php?title=Tutorials for instructions on
how to build Delta3D from source and which CMake build options are available.

NOTE: You will *not* be able to compile the libraries or utilities
      from the Delta3D SDK package, only the examples.  See the Tutorials
      page for help in deciding which Delta3D download is right for you. 


################
# Dependencies #
################

Delta3D makes use of many different external dependencies; some are
required and some are optional.

If you are on Windows and are using a supported version of 
Visual Studio, we have prepared pre-compiled dependency pacakges for you.
These will live in a folder called 'ext' that should reside inside
your Delta3D directory.

Note: if the external dependency is not found by CMake, the Delta3D
projects that rely on that dependency will not be generated.

Please see the latest information on the External Dependencies at:
http://delta3d.org/deltawiki/index.php?title=Dependencies.


#########################
# Environment Variables #
#########################

Delta3D requires environment variables at runtime (but not at compile-time).
The Windows installer will set these up for you, but if you use the zip
package or are on Linux you need to set them youself.

Microsoft Windows
--------------------
Make sure the Delta3D environment variables are set.  Note: Delta3D
libraries are typically contained under a "build" sub-folder that is
named based on the compiler.  The DELTA_LIB and DELTA_BIN variables
may need to be customized based on the installation.

DELTA_ROOT = "C:\Program Files\delta3d"  (or wherever Delta3D is installed)
DELTA_INC = %DELTA_ROOT%\inc;%DELTA_ROOT%\ext\inc;%DELTA_ROOT%\ext\inc\CEGUI
DELTA_LIB = %DELTA_ROOT%\build\lib;%DELTA_ROOT%\ext\lib
DELTA_DATA = %DELTA_ROOT%\data
DELTA_BIN = %DELTA_ROOT%\build\bin;%DELTA_ROOT%\ext\bin

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

   dtHLAGM
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLAGM module is compiled and tested
   against RTI-S 1.3_D18 although any RTI supporting spec 1.3 should work.


   The Python Bindings
   -------------------
   Building the Delta3D Python bindings requires the following additional 
   packages: Python and Boost.Python. 

   See http://delta3d.org/deltawiki/index.php?title=Tutorials for information on 
   how to compile the non-distributed external dependencies required for 
   the Python bindings.
   

   STAGE
   -----
   Building STAGE requires an additional download of:
   -Qt 4.x, the Windows or Linux open-source release (http://qt-project.org/downloads)
   
   See http://delta3d.org/deltawiki/index.php?title=Tutorials for information on 
   how to compile the non-distributed external dependencies required for 
   building STAGE.

