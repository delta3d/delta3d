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
      Delta3D, so you are on your own. Make sure the following env. variables
      are set:
      
      RTI_HOME:       path to RTI installation
      RTI_BUILD_TYPE: the name of the subdirectory under RTI_HOME
                      (e.g. WinNT-4.0-VC6 or 
                      Linux-rh9.0-i386-gcc-3.2.2-opt-mt )
      RTI_RID_FILE:   path to RTI-1.3NGv6.4.rid
                      (e.g. Win32: %DELTA_DATA%/RTI-1.3NGv6.4.rid
                            Linux: $DELTA_DATA/RTI-1.3NGv6.4.rid )
                          
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
- Go to examples/testHLA
- Run 'cmake .'.
- Run 'make'.
- Run 'testHLA'.

Controls
--------
Up     -  Rotate camera up
Down   -  Rotate camera down
Left   -  Rotate camera left
Right  -  Rotate camera right
Esc    -  Exit


