########################################
#                                      #
# Delta3D Examples: testTerrain        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtSOARX

Purpose
-------
To demonstrate the SOARX algorithm using a specific database of
satellite imagery. This database is not supplied with the general
distribute of Delta3D. If you have your own DTED database, then
replace the info contained in the SOARXTerrain node in the file
examples/testTerrain/data/soarxterrain.xml.

Note: dtSOARX is currently not supported under Linux:

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testTerrain.
- Start testTerrain.

Win32 Binary:
- Go to examples/testTerrain/Release.
- Double-click testTerrain.exe.

Controls
--------
+ / -  -  Increase/decrease threshold
[ / ]  -  Increase/decrease detail
;      -  Move back time of day
'      -  Move ahead time of day
Left   -  Turn left
Right  -  Turn right
Up     -  Look up (hold shift to look faster)
Down   -  Look down (hold shift to look faster)
Enter  -  GetScene()->SetNextStatisticsType()
W      -  Toggle Walk motion model.
Space  -  Toggle wireframe view.