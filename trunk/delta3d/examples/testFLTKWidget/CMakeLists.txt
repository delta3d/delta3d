########################################
#
# testFLTKWidget
#
########################################

INCLUDE_DIRECTORIES( BEFORE . )

ADD_EXECUTABLE( testFLTKWidget fl_p51_window fltkwindow viewer helperstructs main )
TARGET_LINK_LIBRARIES( testFLTKWidget dtcore dtabc fltk )



