########################################
#
# Delta3D Examples
#
########################################

PROJECT( examples )

# this will be run from subdirectories, so use two ..'s
INCLUDE_DIRECTORIES( ../../inc ../../ext/inc ) 
LINK_DIRECTORIES( ../../lib ../../ext/lib )

SUBDIRS( testApp testAudio testCharacters testClouds testEffects testFLTKWidget testGUI testInput testMessage testMotionModels testPhysics testSound testTerrain testWeather testWidget testWin )

