########################################
#
# testHLA
#
########################################

PROJECT( testHLA )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include ) 
LINK_DIRECTORIES( ../../lib ../../ext/lib $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib)

ADD_EXECUTABLE( testHLA testhla )
TARGET_LINK_LIBRARIES( testHLA dtcore dtabc dthla )



