########################################
#
# Delta3D Examples
#
########################################

PROJECT( examples )

# this will be run from subdirectories, so use two ..'s
INCLUDE_DIRECTORIES( ../../inc ../../ext/inc ) 
LINK_DIRECTORIES( ../../lib ../../ext/lib )

SUBDIRS( testApp testAudio testClouds testEffects testFLTKWidget testGUI testHLA testInput testMessage testMotionModels testPhysics testSound testTerrain testWeather testWidget testWin )
#add testCharacters testLua testPython
