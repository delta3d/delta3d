########################################
#
# dtUtil: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtutil SHARED domain matrixutil polardecomp )
TARGET_LINK_LIBRARIES( dtutil osg plibsg plibul )

INSTALL_TARGETS( ../lib dtutil )
