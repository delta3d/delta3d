########################################
#
# dtChar: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtchar SHARED character )
TARGET_LINK_LIBRARIES( dtchar dtcore osg osgDB Producer plibsg plibul rbody )

INSTALL_TARGETS( ../lib dtchar )
