########################################
#
# dtVis: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtvis SHARED bspcullcallback )
TARGET_LINK_LIBRARIES( dtvis GL osg osgDB osgUtil )

INSTALL_TARGETS( ../lib dtvis )
