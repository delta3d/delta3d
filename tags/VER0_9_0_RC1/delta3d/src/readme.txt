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

   These variables are NOT necessary to build within a particular
   source tree as we use relative pathing. However, most Delta3D
   applications use these to reference the libraries and data.

2. Check if you have the ext/ and data/ directories already in
   delta3d/.
   
   If you do not, please download the latest version of
   the following packages from Sourceforge and extract them into the
   Delta3D folder:

   delta3d-dependencies 
   delta3d-data        

3. Open VisualStudio/delta_all.sln, build! 

   Alternatively, you build use the other solutions:
   
   VisualStudio/src/delta.sln             : libraries
   VisualStudio/src/*/*.sln               : invdividual libraries
   VisualStudio/src/python/dtpython.sln   : python bindings
   VisualStudio/examples/examples.sln     : examples
   VisualStudio/examples/*/*.sln          : individual examples
   VisualStudio/utilities/*/*.sln         : individual utilities

   dtHLA
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLA module is compiled and tested
   against RTI-S 1.3_D10A although any RTI should work.

   To build the module dtHLA, example testHLA, and utility hlaStealthViewer, 
   you must add your RTI include and lib paths to your global search
   directories using :

   Tools->Options->Projects->VC++ Directories

   or to the project settings.

   See examples/testHLA/readme.txt for more information.
   VS solutions are provided for these apps as:

   src/dtHLA/dthla.sln
   examples/testHLA/testHLA.sln
   utilities/hlaStealthViewer/hlaStealthViewer.sln

   dtPython
   --------  
   The Delta3D Python bindings requires the following additional 
   packages: Python  and boost::Python. 
   
   Install Python 2.3 from:
   http://www.python.org/2.3.4/

   Obtain the Boost library from SourceForge (package 'boost'):
   http://sourceforge.net/projects/boost/

   Decompress Boost.

   Open boost_1_31_0/libs/python/build/VisualStudio/boost_python.dsw.

   Build boost_python.

   Make sure boost_python.dll and boost_python_debug.dll are within 
   a directory in your PATH evironement variable.

   Check if the following environement variables are set:
      PYTHON_ROOT     : directory of Python installation
      PYTHON_LIB_PATH : %PYTHON_ROOT%/libs
      PYTHON_VERSION  : version of your Python installation
      PYTHONPATH      : environement variable contains the directory 
                        with the resulting .pyd libraries (most likely delta3d/bin).

   Build the Delta3D Python bindings:
  - Open VisualStudio/src/python/dtPython.sln
  - Build.

Linux 
-----
Tested on: Fedora Core 2, Fedora Core 3 
SHOULD work on: Debian (Gentoo, Ubuntu), RedHat

1. Run 'glxinfo | grep direct'.
   If you get 'direct rendering: yes' skip to 3.
   Else...

2. Install OpenGL drivers for your video card.

   For info on how to do this, see:

   ATI:   http://www.fedorafaq.org/#radeon
   Nvdia: http://www.fedorafaq.org/#nvidia

3. Set the Delta3D environment variables. 
   
   DELTA_ROOT: path to your Delta3D installation
   DELTA_DATA: $DELTA_ROOT/data
   DELTA_INC:  $DELTA_ROOT/inc:$DELTA_ROOT/ext/inc
   DELTA_LIB:  $DELTA_ROOT/lib:$DELTA_ROOT/ext/lib

   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DELTA_LIB

   Don't forget to 'export' your variables if you are using Bash!

   These variables are NOT necessary to build within a particular
   source tree as we use relative pathing. However, most Delta3D
   applications use these to reference the libraries and data.

4. Check if you have the ext/ and data/ directories already in
   delta3d/.
   
   If you do not, please download the latest version of
   the following packages from Sourceforge and extract them into the
   Delta3D folder:

   delta3d-dependencies 
   delta3d-data 

5. Download SCons from http://www.scons.org .

6. You will notice files called SConstruct and SConscript in the delta3d 
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
    boost=path to your boost installation
    rti=path ro your RTI installation

   dtPython
   --------
   Python is installed on almost Linux distribution. 
   If it ain't get a real distribution ;)

   We've tested with Python 2.3. Version 2.4 SHOULD work, but I haven't
   tested it myself, so no promises.
   
   Boost Python can be found on many distributions as well, but the dev
   package usually instlladed by default. Find an RPM or...

   Install Boost Python (manually):
   - Obtain the Boost library from SourceForge (package 'boost'):
     http://sourceforge.net/projects/boost/
   - Obtain Boost Jam (package 'boost-jam').
   - Decompress the BoostJam archive and place the file 'bjam' in your PATH.
   - Set env. var PYTHON_ROOT to your your python installation (usually /usr or /usr/local)
   - Set env. var PYTHON_VERSION to the 2-part major python version (e.g. 2.3 or 2.4)
   - Go to boost_1_32_0/libs/python/build.
   - Run 'bjam -sTOOLS=gcc'.
   - Copy resulting shared objects to delta3d/ext/lib (non-debug version is
     already supplied, but hey, you want your own, right?):
     'cp -d libboost_python.so* $DELTA_ROOT/ext/lib'
     'cp -d libboost_python_debug.so* $DELTA_ROOT/ext/lib'

   Build the Python bindings for Delta3D engine:
   - Go to the delta3d root directory.
   - Run 'scons boost=/path/to/boost python'

   Set the PYTHONPATH environement variable to contain $DELTA_LIB.

   dtHLA
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLA module is compiled and tested
   against RTI-S 1.3_D10A although any RTI should work.

   Once your have an RTI installed, run:

   'scons rti=/path/to/rti hla'

   and SCons will magically build dtHLA, testHLA, and hlaStealthViewer.
