########################################
#
# testPython
#
########################################

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc /usr/include/python2.3 )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

ADD_EXECUTABLE( testPython testpython )
TARGET_LINK_LIBRARIES( testPython dtcorebindings dtabcbindings )



