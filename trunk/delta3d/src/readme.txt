###############################
#                             #
# Delta3D Source Installation #
#                             #
###############################

Win32 w/ Visual Studio .NET 7.1
-------------------------------

1. Make sure the Delta3D environment variables are set by
   either the Win32 installer or env_var_setup.exe in 
   delta3d/utilities. The following variables are set:
   
   DELTA_ROOT: path to your Delta3D installation
   DELTA_DATA: %DELTA_ROOT%\data
   DELTA_INC:  %DELTA_ROOT%\inc;%DELTA_ROOT%\ext\inc
   DELTA_LIB:  %DELTA_ROOT%\lib;%DELTA_ROOT%\ext\lib
   
   Also add %DELTA_ROOT%\bin to your PATH.

2. Check if you have the ext/ and data/ directories already in
   delta3d/.
   
   If you do not, please download the latest version of
   the following packages from Sourceforge and extract them into the
   Delta3D folder:

   delta3d-dependencies 
   delta3d-data        

3. Open src/delta.sln, build!

4. Examples can be built view the examples/examples.sln
   file.

5. Utilities each have their own solution. Open to build.

   dtHLA
   -----
   HLA-related modules/examples/utilities require RTI, which
   we cannot distribute. To build the module dtHLA, example
   testHLA, and utility hlaStealthViewer, RTI must be installed
   with the RTI_HOME, RTI_BUILD_TYPE, and RTI_RID_FILE envionment
   variables set. See examples/testHLA/readme.txt for more information.
   VS solutions are provided for these apps as:

   src/dtHLA/dthla.sln
   examples/testHLA/testHLA.sln
   utilities/hlaStealthViewer/hlaStealthViewer.sln

   dtPython
   --------  
   The Delta3D Python bindings requires the following additional 
   packages: Python  and boost::Python. 
   See examples/testPython/readme.txt for more information.

Linux 
-----
Tested on: Fedora Core 2, Fedora Core 3 
Kernel <= 2.6.10-1.741_FC3

1. Run 'glxinfo | grep direct'.
   If you get 'direct rendering: yes' skip to 3.
   Else...

2. Install OpenGL drivers for your video card.

   For info on how to do this, see:

   ATI:   http://www.fedorafaq.org/#radeon
   Nvdia: http://www.fedorafaq.org/#nvidia

3. Set the Delta3D environment variables. There is no
   installer cause this is Linux, and you're a power user,
   right? The following variables need to be set:
   
   DELTA_ROOT: path to your Delta3D installation
   DELTA_DATA: $DELTA_ROOT/data
   DELTA_INC:  $DELTA_ROOT/inc:$DELTA_ROOT/ext/inc
   DELTA_LIB:  $DELTA_ROOT/lib:$DELTA_ROOT/ext/lib

   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DELTA_LIB

   Don't forget to 'export' your variables if you are using Bash!
   
4. Check if you have the ext/ and data/ directories already in
   delta3d/.
   
   If you do not, please download the latest version of
   the following packages from Sourceforge and extract them into the
   Delta3D folder:

   delta3d-dependencies 
   delta3d-data 

5. Download CMake from http://www.cmake.org .
   We use CMake to synconize cross-platform builds of Delta3D.

6. You will notice files called CMakeLists.txt in the following directories:

   src/
   examples/
   utilities/bspCompiler
   utilities/psEditor
   utilities/Viewer

   These files are high-level instructions that tell CMake how to genereate 
   Makefiles for your configuration. In each of these directories run:

   cmake . 
   make

   to build the apprpropriate files. 

   To build the Delta3D source, also run 'make install' to place the shared
   libraries in the ../lib folder.

   dtPython
   --------
   Follow instructions in examples/testPython/readme.txt.

   dtHLA
   -----
   Follow instructions in examples/testHLA/readme.txt

