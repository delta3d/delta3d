########################################
#
# Delta3D: CMakeLists.txt
#
########################################
# 
# CMake uses this file to automatically generate Linux Makefiles.
#

PROJECT( DELTA )

SET( CMAKE_INSTALL_PREFIX ${DELTA_SOURCE_DIR}/ )

# will be run from subdirs, so use ../.. instead of ..
INCLUDE_DIRECTORIES( ../../inc ../../ext/inc )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

IF(EXISTS /usr/X11R6/lib )
LINK_DIRECTORIES( /usr/X11R6/lib )
ENDIF(EXISTS /usr/X11R6/lib )

SUBDIRS( dtCore dtABC dtAudio dtChar dtUtil dtVis gui_fl )

