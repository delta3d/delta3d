########################################
#
# testPython
#
########################################

INCLUDE_DIRECTORIES( /usr/include/python2.3 )

ADD_EXECUTABLE( testPython main )
TARGET_LINK_LIBRARIES( testPython dtcorebindings dtabcbindings )



