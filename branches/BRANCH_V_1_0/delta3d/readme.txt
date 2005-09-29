###########################################################################
#                                                                         #
# Delta3D: The Open-Source Engine Supported by the United States Military #
#                                                                         #
###########################################################################

Welcome to Delta3D ! We hope to get you up and running with Delta3D as
quickly as possible. If you only wish run pre-built applications and have
already used the Win32 installer, you are ready to rock. 

Go ahead and try out the examples by running the executables found in
the bin folder.

There are also several useful utitilies also found in the bin/ folder.
See the readme.txt in /utilities for more details.

If you are a developer you'll want to build from the source
which we provide with all our distributions. Please follow instructions
in src/readme.txt to build the source.

Thanks for using Delta3D! Please visit our webpage at http://www.delta3d.org
and don't hesistate to use the forums for any problems you may have. We'll do
our best to respond as quick as possible.

Sincerely,

The Delta3D Team

########################
#                      #
# Building From Source #
#                      #
########################

See env_vars.txt for information on setting up your Delta3D environment variables. 
Check if you have the ext/ and data/ directories already in delta3d/. If you do not 
have them, please download the latest version of the following packages for your 
platform/compiler from SourceForge and extract them into the Delta3D folder:

   delta3d-dependencies 
   delta3d-data

Win32 w/ Visual Studio .NET 7.1
-------------------------------

1. Open VisualStudio/delta.sln, build! This will generate the essential Delta3D
   libraries that can be built with our distributed dependencies.

2. The rest of the components can be build with their own solutions
   (dependencies that we do not distribute are listed in parentheses):
   
   VisualStudio/src/dtHLA/dthla.sln          : HLA library (RTI)
   VisualStudio/src/dtScript/dtscript.sln    : library to run python scripts (Python)
   VisualStudio/src/python/dtpython.sln      : python bindings (Python, Boost.Python)
   VisualStudio/src/python/dthlabindings.sln : python HLA bindings (Python, BoostPython, RTI)
   VisualStudio/examples/*/*.sln             : individual examples
   VisualStudio/utilities/*/*.sln            : individual utilities
 
   Or just build EVERYTHING (make sure you have Python, BoostPython, RTI):
   VisualStudio/delta_all.sln  

   dtHLA
   -----
   Requries RTI package to be installed! We cannot distribute RTI with
   Delta3D, so you are on your own. The dtHLA module is compiled and tested
   against RTI-S 1.3_D11A although any RTI should work.

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
   The Delta3D Python bindings requires the following additional 
   packages: Python and Boost.Python. 

   We've tested with Python 2.3 and 2.4 - currently the project files refer
   to python24.lib.

   Install Python from:
   http://www.python.org/

   Obtain the Boost library from SourceForge (package 'boost'):
   http://sourceforge.net/projects/boost/

   *NOTE*: We do not support Boost 1.33 at this time!
   
   Decompress Boost.

   Open boost_1_32_0/libs/python/build/VisualStudio/boost_python.dsw.

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
  - Open VisualStudio/src/python/dtpython.sln
  - Build.

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
    boost=path to your boost installation
    rti=path ro your RTI installation

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
   - Go to boost_1_32_0/libs/python/build.
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
