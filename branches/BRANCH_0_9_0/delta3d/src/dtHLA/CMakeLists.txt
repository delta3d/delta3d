########################################
#
# dtHLA: CMakeLists.txt
#
########################################

# Note! You must have RTI installed for dthla to build. 
# We cannot distribute RTI, so you are on your own.

PROJECT( DT_HLA )

SET( CMAKE_INSTALL_PREFIX ${DT_HLA_SOURCE_DIR}/ )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

INCLUDE_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
LINK_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )

ADD_LIBRARY( dthla SHARED dis_types entity rticonnection )
TARGET_LINK_LIBRARIES( dthla dtcore ode OpenThreads osg osgDB osgUtil osgSim osgText plibsg plibul Producer RTI-NG fedtime tinyxml )

INSTALL_TARGETS( ../../lib dthla )
