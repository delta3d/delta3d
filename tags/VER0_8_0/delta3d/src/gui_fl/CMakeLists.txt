########################################
#
# gui_fl: CMakeLists.txt
#
########################################

ADD_LIBRARY( gui_fl SHARED gui_fltk guimgr )
TARGET_LINK_LIBRARIES( gui_fl dtcore fltk plibsg plibul OpenThreads osg Producer )

INSTALL_TARGETS( ../lib gui_fl )
