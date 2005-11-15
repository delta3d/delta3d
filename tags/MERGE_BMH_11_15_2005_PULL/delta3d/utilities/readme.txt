##############################
#                            #
# Delta3D: Utilities         #
#                            #
##############################

A collection of useful utilities for a Delta3D user/developer. Please follow
instructions in the main readme.txt for building from source. Otherwise,
use the .exe in the Release folders.

bspCompiler
----------
Uses Binary Space Partitioning and Potenially Visible Sets to reduce the number
of rendered nodes at runtime. Basically trades extra time to BSP-compiler for
faster a faster runtime. To use, from the command line:

bspCompiler fileIn fileOut

For examples try using bsp_gamelevel.ive from data/:
- Open bsp_gamelevel.ive in the Delta3D Viewer (see below). Note the wireframe
  model and how you can see any part from any room in the scene.
- Run 'bspCompiler bsp_gamelevel.ive bsp_gamelevel.osg'
- Open bsp_gamelevel.osg in the Delta3D Viewer. Go to wireframe mode and notice
  now that other rooms are not rendered from the point of view of your current
  room.

Note: make sure the output file format is .osg or .ive.

editor
------
This is the long awaited STAGE (aka Simluation, Traning, And Game Editor).
Check out the main readme.txt in the Delta3d root directory  for
instructions on how to build it on Windows.

For linux, simply have QTDIR set to your Qt installation and do a:
'scons .', which should build everything Delta3D has to offer.

hlaStealthViewer
----------------
An simple tool that plots the location of HLA entities on a globe. Use this
with testHLA to verify the dtHLA module is working. Note: The RTI implementation
of HLA is not distrubted with Delta3D. Please see the main readme.txt in the
Delta3d root directory for more information.

psEditor
--------
A Particle System editor. Use this tool to create OSG particle effects in
real-time. 

viewer
------
Opens and views any OSG-compatible model.
