########################################
#
# hlaStealthViewer
#
########################################

PROJECT( HLA_STEALTH_VIEWER )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
LINK_DIRECTORIES( ../../lib ../../ext/lib $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )

ADD_EXECUTABLE( hlaStealthViewer main )
TARGET_LINK_LIBRARIES( hlaStealthViewer dtcore dtabc dthla )



