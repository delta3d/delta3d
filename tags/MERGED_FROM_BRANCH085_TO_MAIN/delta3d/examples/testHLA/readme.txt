########################################
#                                      #
# Delta3D Examples: testHLA            #
#                                      #
########################################

Uses modules: dtCore, dtHLA

Purpose
-------
To create/join an HLA federation and register an entity (a cessna flying in a
circle) as well as effects. The utility hlaStealthViewer can be used to join
the same federation and verify the location of the cessna on its map.

Note: Requries RTI package to be installed! We cannot distribute RTI with
      Delta3D, so you are on your own.  The dtHLA module is compiled and tested
      against RTI-S 1.3_D10A.  

Windows users:
- If you use a different RTI, you must replace the library name listed in 
  the dthla project settings and supply the include and library paths.

For Linux RTI users:
- Set the following environment variables:
  RTI_HOME=path to your RTI installation
  RTI_INCLUDE=path to your RTI include directory (containing RTI.hh, etc.)
  RTI_LIB=path to your RTI lib directory
                          
Instructions
------------
Win32 Source: 
- Open examples/testHLA/testHLA.sln.
- Build testHLA.
- Start testHLA.

Win32 Binary:
- Go to examples/testHLA/Release.
- Double-click testHLA.exe.

Linux Source:
- Go to the delta3d root directory.
- Run 'scons hla'
- This will build dtHLA, testHLA, and hlaStealthViewer.

Controls
--------
Up     -  Rotate camera up
Down   -  Rotate camera down
Left   -  Rotate camera left
Right  -  Rotate camera right
Esc    -  Exit


