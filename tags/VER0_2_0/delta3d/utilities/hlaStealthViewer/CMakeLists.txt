########################################
#
# hlaStealthViewer
#
########################################

PROJECT( HLA_STEALTH_VIEWER )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc ../../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/include )
LINK_DIRECTORIES( ../../lib ../../ext/lib ../../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/lib )

ADD_EXECUTABLE( hlaStealthViewer main )
TARGET_LINK_LIBRARIES( hlaStealthViewer dtcore dtabc dthla )



