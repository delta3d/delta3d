########################################
#
# Delta3D Examples
#
########################################

PROJECT( examples )

# this will be run from subdirectories, so use two ..'s
INCLUDE_DIRECTORIES( ../../inc ../../ext/inc ) 
LINK_DIRECTORIES( ../../lib ../../ext/lib )

SUBDIRS( testApp testAudio testCharacters testClouds testEffects testGUI testInput testLights testMotionModels testPhysics testSound testWeather testWidget testWin )

