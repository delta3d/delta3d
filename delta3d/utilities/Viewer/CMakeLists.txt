########################################
#
# Viewer
#
########################################

PROJECT( VIEWER )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

ADD_EXECUTABLE( viewer main viewer.cpp UserInterface viewstate viewwindow )
TARGET_LINK_LIBRARIES( viewer dtcore dtabc fltk GL osgFX plibul )



