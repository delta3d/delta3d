########################################
#
# testHLA
#
########################################

INCLUDE_DIRECTORIES( ../../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/include )

LINK_DIRECTORIES( ../../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/lib )

ADD_EXECUTABLE( testHLA main )
TARGET_LINK_LIBRARIES( testHLA dtcore dtabc dthla )



