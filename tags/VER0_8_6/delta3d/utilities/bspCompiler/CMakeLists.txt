########################################
#
# bspCompiler
#
########################################

PROJECT( BSP_COMPILER )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

ADD_EXECUTABLE( bspCompiler main splitgeode )
TARGET_LINK_LIBRARIES( bspCompiler dtvis fltk osg osgDB osgParticle osgText osgUtil plibjs plibsg plibul tinyxml )



