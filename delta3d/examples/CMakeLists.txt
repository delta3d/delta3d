########################################
#
# Delta3D Examples
#
########################################

PROJECT( examples )

SET( DELTA_HOME /home/cwosborn/delta3d )

INCLUDE_DIRECTORIES( ${DELTA_HOME}/inc ${DELTA_HOME}/ext/inc )
LINK_DIRECTORIES( ${DELTA_HOME}/lib ${DELTA_HOME}/ext/lib )

SUBDIRS( testApp testClouds testEffects testFLTKWidget testGUI testInput testMessage testMotionModels testPhysics testTerrain testWeather testWidget testWin )
#add testCharacters testHLA testLua testPython
