########################################
#                                      #
# dtSoarx  Demo                        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtSOARX

Purpose
-------
To demonstrate the SOARX algorithm using a specific database of
satellite imagery. This database is not supplied with the general
distribute of Delta3D. If you have your own DTED database, then
replace the info contained in the SOARXTerrain node in the file
examples/testSoarx/data/soarxterrain.xml.

Note: dtSOARX is currently not supported under Linux:

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testSoarx.
- Start testSoarx.

Win32 Binary:
- Go to examples/testSoarx/Release.
- Double-click testSoarx.exe.

Controls
--------
+ / -  -  Increase/decrease threshold
[ / ]  -  Increase/decrease detail

Mouse Left Bt - Look around
w             -  Move forward
s             -  Move backward
Shift         -  Turbo
Enter  -  Display next statistic
Space  -  Toggle wireframe view.