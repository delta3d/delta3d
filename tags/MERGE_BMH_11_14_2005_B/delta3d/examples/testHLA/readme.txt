########################################
#                                      #
# Delta3D Examples: testHLA            #
#                                      #
########################################

Uses modules: dtCore, dtHLA, dtABC, dtUtil

Purpose
-------
To create/join an HLA federation and register an entity (a cessna flying in a
circle) as well as effects. The utility hlaStealthViewer can be used to join
the same federation and verify the location of the cessna on its map.

Note: Requries RTI package to be installed! We cannot distribute RTI with
      Delta3D, so you are on your own.  The dtHLA module is compiled and tested
      against RTI-S 1.3_D11A although any RTI should work.

Windows users:
- If you use a different RTI, you must provide the library name listed in 
  the dtHLA project settings and supply the include and library paths.

For Linux RTI users:
- Run 'scons rti=/path/to/rti hla'
                          
Instructions
------------
Win32 Source: 
- Open examples\testHLA\testHLA.sln.
- Build testHLA.
- Start testHLA.

Win32 Binary:
- Go to bin.
- Double-click testHLA.exe.

Linux Source:
- Go to the delta3d root directory.
- Run 'scons rti=/path/to/rti hla'
- This will build dtHLA, testHLA, and hlaStealthViewer.

Linux Binary:
- cd bin
- ./hlaStealthViewer & 
- ./testHLA

Controls
--------
Up     -  Rotate camera up
Down   -  Rotate camera down
Left   -  Rotate camera left
Right  -  Rotate camera right
Esc    -  Exit
